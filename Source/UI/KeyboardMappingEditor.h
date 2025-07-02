/*
    KeyboardMappingEditor.h
    ---------------------------------------------------------------------------
    A rock‑solid overlay component for JUCE that lets the user draw & edit sample
    zones directly on top of a juce::MidiKeyboardComponent.

      • X‑Axis → MIDI note range (startNote … endNote)
      • Y‑Axis → Velocity range (velLow … velHigh)

    Integration (SAM plugin‑style):
        zones.push_back ({ 60, 72, 40, 100, {} });             // optional seed
        mappingEditor = std::make_unique<KeyboardMappingEditor>(*keyboard, zones);
        addAndMakeVisible(mappingEditor.get());
        mappingEditor->toFront(true);

    Zones can be freely edited by dragging their edges or their full body.
    Active zone is highlighted. Handles are visible at edges/corners.

    Author: Matthias Püski / OpenAI helper
*/

#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <algorithm>

//============================================================================
struct SampleZone
{
    int startNote = 60;   // inclusive (0–127)
    int endNote = 60;   // inclusive
    int velLow = 1;    // 1–127
    int velHigh = 127;  // 1–127
    int note = 60;
};

//============================================================================
class KeyboardMappingEditor : public juce::Component
{
public:
    KeyboardMappingEditor(juce::MidiKeyboardComponent& kb,
        std::vector<SampleZone>& zoneData)
        : keyboard(kb), zones(zoneData)
    {
        setInterceptsMouseClicks(true, true);
    }

    //------------------------------------------------------------------------
    void paint(juce::Graphics& g) override
    {
        if (getWidth() <= 0 || getHeight() <= 0)
            return;

        for (size_t i = 0; i < zones.size(); ++i)
        {
            const auto& z = zones[i];
            if (!isValid(z))
                continue;

            const int x1 = getKeyPositionX(z.startNote);
            const int x2 = getKeyPositionX(z.endNote + 1); // include full key
            const int y1 = velToY(z.velHigh + 1);
            const int y2 = velToY(z.velLow);

            if (x1 < 0 || x2 <= x1)
                continue;

            juce::Rectangle<float> r((float)x1,
                (float)y1,
                (float)(x2 - x1),
                (float)(y2 - y1));

            const bool isActive = (int)i == drag.idx;

            g.setColour((isActive ? juce::Colours::orange : juce::Colours::skyblue).withAlpha(0.35f));
            g.fillRect(r);
            g.setColour((isActive ? juce::Colours::orange : juce::Colours::skyblue).darker(0.4f));
            g.drawRect(r, 1.0f);

            // draw handles if active
            if (isActive)
            {
                const float hs = 6.0f; // handle size
                g.setColour(juce::Colours::black);
                g.fillRect (juce::Rectangle<float> (hs, hs).withCentre (r.getTopLeft()));
                g.fillRect (juce::Rectangle<float> (hs, hs).withCentre (r.getTopRight()));
                g.fillRect (juce::Rectangle<float> (hs, hs).withCentre (r.getBottomLeft()));
                g.fillRect (juce::Rectangle<float> (hs, hs).withCentre (r.getBottomRight()));
            }
        }
    }

    //------------------------------------------------------------------------
    void mouseDown(const juce::MouseEvent& e) override
    {
        const int note = xToKey(e.position.x);
        const int vel = yToVel(e.position.y);

        drag.idx = hitTest(note, vel, drag.edge);
        drag.start = { note, vel };

        repaint();
    }

    void mouseDrag(const juce::MouseEvent& e) override
    {
        if (drag.idx < 0)
            return;

        auto& z = zones[(size_t)drag.idx];

        const int note = juce::jlimit(0, 127, xToKey(e.position.x));
        const int vel = juce::jlimit(1, 127, yToVel(e.position.y));

        switch (drag.edge)
        {
        case Edge::left:   z.startNote = note; break;
        case Edge::right:  z.endNote = note; break;
        case Edge::top:    z.velHigh = vel;  break;
        case Edge::bottom: z.velLow = vel;  break;
        case Edge::none:
        {
            int deltaNote = note - drag.start.x;
            int deltaVel = vel - drag.start.y;
            z.startNote = juce::jlimit(0, 127, z.startNote + deltaNote);
            z.endNote = juce::jlimit(0, 127, z.endNote + deltaNote);
            z.velLow = juce::jlimit(1, 127, z.velLow + deltaVel);
            z.velHigh = juce::jlimit(1, 127, z.velHigh + deltaVel);
            drag.start = { note, vel };
            break;
        }
        }

        normalise(z);
        repaint();
    }

    void mouseDoubleClick(const juce::MouseEvent& e) override
    {
        const int note = xToKey(e.position.x);
        const int vel = yToVel(e.position.y);
        Edge dummy;

        const int minVel = juce::jlimit(1, 118, vel - 5); // max 118, damit +9 nicht >127
        const int maxVel = minVel + 9;

        const int hit = hitTest(note, vel, dummy);

        if (e.mods.isCtrlDown())
        {
            // STRG + Doppelklick → Zone löschen
            if (hit >= 0)
            {
                zones.erase(zones.begin() + hit);
                drag.idx = -1;
                repaint();
            }
        }
        else
        {
            // Doppelklick → neue Zone anlegen (nur wenn leer)
            if (hit < 0)
            {
                zones.push_back({ note, note, minVel, maxVel, {} });
                drag.idx = (int)zones.size() - 1;
                drag.edge = Edge::right;
                repaint();
            }
        }
    }

    void mouseUp(const juce::MouseEvent&) override { drag.idx = -1; }

private:
    //============================================================================
    static bool isBlackKey(int midiNote)
    {
        if (midiNote < 0 || midiNote > 127) return false;
        constexpr int black[5] = { 1, 3, 6, 8, 10 };
        return std::find(std::begin(black), std::end(black), midiNote % 12) != std::end(black);
    }

    static bool isValid(const SampleZone& z)
    {
        return z.startNote >= 0 && z.endNote <= 127 && z.startNote <= z.endNote &&
            z.velLow >= 1 && z.velHigh <= 127 && z.velLow <= z.velHigh;
    }

    static int countWhiteKeys(int from, int to)
    {
        int c = 0; for (int n = from; n <= to; ++n) if (!isBlackKey(n)) ++c; return c;
    }

    int getWhiteKeyWidth() const
    {
        const int totalWhite = countWhiteKeys(keyboard.getLowestVisibleKey(), 127);
        return totalWhite > 0 ? juce::roundToInt(getWidth() / (float)totalWhite) : 0;
    }

    int getHighestVisibleKey() const
    {
        const int wkw = getWhiteKeyWidth();
        int x = 0, note = keyboard.getLowestVisibleKey();
        while (note <= 127)
        {
            if (!isBlackKey(note)) x += wkw;
            if (x >= getWidth()) return note;
            ++note;
        }
        return 127;
    }

    int getKeyPositionX(int midiNote) const
    {
        if (midiNote < keyboard.getLowestVisibleKey()) return -1;
        const int wkw = getWhiteKeyWidth();
        if (wkw == 0) return -1;

        int whitesLeft = countWhiteKeys(keyboard.getLowestVisibleKey(), midiNote - 1);
        int x = whitesLeft * wkw;

        if (isBlackKey(midiNote)) x += int(wkw * 0.75f);
        return x;
    }

    int getKeyWidth(int midiNote) const
    {
        const int wkw = getWhiteKeyWidth();
        return isBlackKey(midiNote) ? wkw / 2 : wkw;
    }

    int xToKey(float x) const
    {
        const int low = keyboard.getLowestVisibleKey();
        const int high = getHighestVisibleKey();
        for (int note = low; note <= high; ++note)
        {
            const int keyX = getKeyPositionX(note);
            const int w = getKeyWidth(note);
            if (x >= keyX && x < keyX + w) return note;
        }
        return low;
    }

    int velToY(int vel) const { return juce::jmap(vel, 1, 127, getHeight(), 0); }
    int yToVel(float y) const { return juce::jmap(int(y), getHeight(), 0, 1, 127); }

    static void normalise(SampleZone& z)
    {
        if (z.startNote > z.endNote)
            std::swap(z.startNote, z.endNote);
        if (z.velLow > z.velHigh)
            std::swap(z.velLow, z.velHigh);

        // Mindestgröße: Velocity-Range mindestens 10
        const int minVelSize = 10;
        if (z.velHigh - z.velLow + 1 < minVelSize)
        {
            int center = (z.velLow + z.velHigh) / 2;
            z.velLow = juce::jlimit(1, 127 - minVelSize + 1, center - minVelSize / 2);
            z.velHigh = juce::jlimit(z.velLow + minVelSize - 1, 127, z.velLow + minVelSize - 1);
        }
    }

    enum class Edge { none, left, right, top, bottom };

    int hitTest(int note, int vel, Edge& out) const
    {
        const int px = 4;
        for (size_t i = 0; i < zones.size(); ++i)
        {
            const auto& z = zones[i];
            if (!isValid(z)) continue;
            if (note < z.startNote || note > z.endNote || vel < z.velLow || vel > z.velHigh) continue;

            const int keyX = getKeyPositionX(note);

            if (std::abs(keyX - getKeyPositionX(z.startNote)) < px) out = Edge::left;
            else if (std::abs(keyX - getKeyPositionX(z.endNote)) < px) out = Edge::right;
            else if (std::abs(vel - z.velLow) < 2) out = Edge::bottom;
            else if (std::abs(vel - z.velHigh) < 2) out = Edge::top;
            else out = Edge::none;

            return (int)i;
        }
        out = Edge::none;
        return -1;
    }

    struct DragInfo { int idx = -1; Edge edge = Edge::none; juce::Point<int> start; } drag;

    juce::MidiKeyboardComponent& keyboard;
    std::vector<SampleZone>& zones;

};

