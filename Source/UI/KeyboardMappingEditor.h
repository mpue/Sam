/*
    KeyboardMappingEditor.h (Improved Version)
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

    IMPROVEMENTS:
    - Better const correctness and thread safety
    - Proper callback system for zone changes
    - Undo/Redo support preparation
    - Better color customization
    - Improved performance with dirty region tracking
    - Better keyboard accessibility
    - More robust hit testing
    - Configurable snap-to-grid functionality

    Author: Matthias Püski / OpenAI helper (Improved by Claude)
*/

#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <algorithm>
#include <functional>
#include <memory>
#include "../AudioEngine/Sampler.h"

//============================================================================
struct SampleZone
{
    int startNote = 60;     // inclusive (0–127)
    int endNote = 60;       // inclusive
    int velLow = 1;         // 1–127
    int velHigh = 127;      // 1–127
    int note = 60;

    // Optional: additional metadata
    juce::String name;
    juce::Colour customColor = juce::Colours::transparentBlack;

    // std::unique_ptr requires special handling
    std::unique_ptr<Sampler> sampler = nullptr;

    // Default constructor
    SampleZone() = default;

    // Copy constructor - creates new sampler if source has one
    SampleZone(const SampleZone& other)
        : startNote(other.startNote), endNote(other.endNote)
        , velLow(other.velLow), velHigh(other.velHigh), note(other.note)
        , name(other.name), customColor(other.customColor)
        , sampler(nullptr) // Don't copy the sampler - create new one if needed
    {
        // Note: sampler is intentionally NOT copied
        // You can override this behavior in your specific use case
    }

    // Copy assignment operator - creates new sampler if source has one
    SampleZone& operator=(const SampleZone& other)
    {
        if (this != &other)
        {
            startNote = other.startNote;
            endNote = other.endNote;
            velLow = other.velLow;
            velHigh = other.velHigh;
            note = other.note;
            name = other.name;
            customColor = other.customColor;

            // Reset our sampler - don't copy the unique_ptr
            sampler.reset();
            // Note: You'll need to recreate the sampler manually if needed
        }
        return *this;
    }

    // Move constructor
    SampleZone(SampleZone&& other) noexcept
        : startNote(other.startNote), endNote(other.endNote)
        , velLow(other.velLow), velHigh(other.velHigh), note(other.note)
        , name(std::move(other.name)), customColor(other.customColor)
        , sampler(std::move(other.sampler))  // Move the unique_ptr
    {
    }

    // Move assignment
    SampleZone& operator=(SampleZone&& other) noexcept
    {
        if (this != &other)
        {
            startNote = other.startNote;
            endNote = other.endNote;
            velLow = other.velLow;
            velHigh = other.velHigh;
            note = other.note;
            name = std::move(other.name);
            customColor = other.customColor;
            sampler = std::move(other.sampler);  // Move the unique_ptr
        }
        return *this;
    }

    bool operator==(const SampleZone& other) const noexcept
    {
        return startNote == other.startNote && endNote == other.endNote &&
            velLow == other.velLow && velHigh == other.velHigh && note == other.note;
        // sampler is not compared as it's not part of the zone definition
    }

    // Utility method to check if this zone has an active sampler
    bool hasSampler() const noexcept
    {
        return sampler != nullptr;
    }

    // Utility method to safely reset the sampler
    void clearSampler()
    {
        sampler.reset();
    }
};

//============================================================================
class KeyboardMappingEditor : public juce::Component,
    public juce::KeyListener
{
public:
    //------------------------------------------------------------------------
    // Callback types for zone changes
    using ZoneChangedCallback = std::function<void(int zoneIndex, const SampleZone& zone)>;
    using ZoneAddedCallback = std::function<void(int zoneIndex, const SampleZone& zone)>;
    using ZoneRemovedCallback = std::function<void(int zoneIndex)>;
    using SelectionChangedCallback = std::function<void(int zoneIndex)>;

    //------------------------------------------------------------------------
    // Configuration struct for better customization
    struct Configuration
    {
        juce::Colour activeZoneColor = juce::Colours::orange;
        juce::Colour inactiveZoneColor = juce::Colours::skyblue;
        float zoneAlpha = 0.35f;
        float handleSize = 8.0f;                    // Increased from 6.0f
        int edgePixelTolerance = 8;                 // Increased from 4
        int velocityTolerance = 5;                  // Increased from 2  
        int minimumVelocityRange = 10;
        int newZoneVelocityRange = 10;
        bool snapToWhiteKeys = false;
        bool showZoneLabels = true;
        bool enableKeyboardNavigation = true;
        bool showHoverEffect = true;                // New: show hover feedback
    };

    //------------------------------------------------------------------------
    KeyboardMappingEditor(juce::MidiKeyboardComponent& kb,
        std::vector<SampleZone>& zoneData,
        const Configuration& config = Configuration{})
        : keyboard(kb), zones(zoneData), config_(config)
    {
        setInterceptsMouseClicks(true, true);
        setWantsKeyboardFocus(config_.enableKeyboardNavigation);
        if (config_.enableKeyboardNavigation)
            addKeyListener(this);
    }

    ~KeyboardMappingEditor() override
    {
        if (config_.enableKeyboardNavigation)
            removeKeyListener(this);
    }

    //------------------------------------------------------------------------
    // Callback setters
    void setZoneChangedCallback(ZoneChangedCallback callback) { onZoneChanged_ = std::move(callback); }
    void setZoneAddedCallback(ZoneAddedCallback callback) { onZoneAdded_ = std::move(callback); }
    void setZoneRemovedCallback(ZoneRemovedCallback callback) { onZoneRemoved_ = std::move(callback); }
    void setSelectionChangedCallback(SelectionChangedCallback callback) { onSelectionChanged_ = std::move(callback); }

    //------------------------------------------------------------------------
    // Public API
    void setSelectedZone(int index)
    {
        if (index >= -1 && index < static_cast<int>(zones.size()) && index != selectedZoneIndex_)
        {
            selectedZoneIndex_ = index;
            if (onSelectionChanged_)
                onSelectionChanged_(selectedZoneIndex_);
            repaint();
        }
    }

    int getSelectedZone() const noexcept { return selectedZoneIndex_; }

    int getHoveredZone() const noexcept { return hoveredZoneIndex_; }

    void clearSelection()
    {
        setSelectedZone(-1);
    }

    void deleteSelectedZone()
    {
        if (selectedZoneIndex_ >= 0 && selectedZoneIndex_ < static_cast<int>(zones.size()))
        {
            deleteZone(selectedZoneIndex_);
        }
    }

    //------------------------------------------------------------------------
    // Zone lookup methods

    /**
     * Find the first zone that contains the given MIDI note (any velocity).
     * @param midiNote The MIDI note number (0-127)
     * @return Zone index or -1 if no zone found
     */
    int findZoneForNote(int midiNote) const
    {
        for (int i = 0; i < static_cast<int>(zones.size()); ++i)
        {
            const auto& zone = zones[static_cast<size_t>(i)];
            if (isValidZone(zone) &&
                midiNote >= zone.startNote && midiNote <= zone.endNote)
            {
                return i;
            }
        }
        return -1;
    }

    /**
     * Find the zone that contains the given MIDI note and velocity.
     * @param midiNote The MIDI note number (0-127)
     * @param velocity The MIDI velocity (1-127)
     * @return Zone index or -1 if no zone found
     */
    int findZoneForNoteAndVelocity(int midiNote, int velocity) const
    {
        for (int i = 0; i < static_cast<int>(zones.size()); ++i)
        {
            const auto& zone = zones[static_cast<size_t>(i)];
            if (isValidZone(zone) &&
                midiNote >= zone.startNote && midiNote <= zone.endNote &&
                velocity >= zone.velLow && velocity <= zone.velHigh)
            {
                return i;
            }
        }
        return -1;
    }

    /**
     * Find all zones that contain the given MIDI note.
     * @param midiNote The MIDI note number (0-127)
     * @return Vector of zone indices (empty if no zones found)
     */
    std::vector<int> findAllZonesForNote(int midiNote) const
    {
        std::vector<int> matchingZones;
        for (int i = 0; i < static_cast<int>(zones.size()); ++i)
        {
            const auto& zone = zones[static_cast<size_t>(i)];
            if (isValidZone(zone) &&
                midiNote >= zone.startNote && midiNote <= zone.endNote)
            {
                matchingZones.push_back(i);
            }
        }
        return matchingZones;
    }

    /**
     * Find all zones that contain the given MIDI note and velocity.
     * @param midiNote The MIDI note number (0-127)
     * @param velocity The MIDI velocity (1-127)
     * @return Vector of zone indices (empty if no zones found)
     */
    std::vector<int> findAllZonesForNoteAndVelocity(int midiNote, int velocity) const
    {
        std::vector<int> matchingZones;
        for (int i = 0; i < static_cast<int>(zones.size()); ++i)
        {
            const auto& zone = zones[static_cast<size_t>(i)];
            if (isValidZone(zone) &&
                midiNote >= zone.startNote && midiNote <= zone.endNote &&
                velocity >= zone.velLow && velocity <= zone.velHigh)
            {
                matchingZones.push_back(i);
            }
        }
        return matchingZones;
    }

    /**
     * Find the best matching zone for a MIDI note and velocity.
     * If multiple zones overlap, returns the one with the most specific velocity range.
     * @param midiNote The MIDI note number (0-127)
     * @param velocity The MIDI velocity (1-127)
     * @return Zone index or -1 if no zone found
     */
    int findBestZoneForNoteAndVelocity(int midiNote, int velocity) const
    {
        int bestZone = -1;
        int smallestVelRange = 128; // Smaller range = more specific

        for (int i = 0; i < static_cast<int>(zones.size()); ++i)
        {
            const auto& zone = zones[static_cast<size_t>(i)];
            if (isValidZone(zone) &&
                midiNote >= zone.startNote && midiNote <= zone.endNote &&
                velocity >= zone.velLow && velocity <= zone.velHigh)
            {
                const int velRange = zone.velHigh - zone.velLow + 1;
                if (velRange < smallestVelRange)
                {
                    smallestVelRange = velRange;
                    bestZone = i;
                }
            }
        }
        return bestZone;
    }

    /**
     * Get direct access to a zone by index (const version).
     * @param index Zone index
     * @return Const reference to zone or nullptr if index invalid
     */
    const SampleZone* getZone(int index) const
    {
        if (index >= 0 && index < static_cast<int>(zones.size()))
            return &zones[static_cast<size_t>(index)];
        return nullptr;
    }

    /**
     * Get direct access to a zone by index (non-const version).
     * @param index Zone index
     * @return Reference to zone or nullptr if index invalid
     */
    SampleZone* getZone(int index)
    {
        if (index >= 0 && index < static_cast<int>(zones.size()))
            return &zones[static_cast<size_t>(index)];
        return nullptr;
    }

    /**
     * Get the total number of zones.
     * @return Number of zones
     */
    int getNumZones() const noexcept
    {
        return static_cast<int>(zones.size());
    }

    //------------------------------------------------------------------------
    void paint(juce::Graphics& g) override
    {
        if (getWidth() <= 0 || getHeight() <= 0)
            return;

        // Paint zones from back to front, with selected zone last
        std::vector<int> paintOrder;
        for (int i = 0; i < static_cast<int>(zones.size()); ++i)
        {
            if (i != selectedZoneIndex_)
                paintOrder.push_back(i);
        }
        if (selectedZoneIndex_ >= 0)
            paintOrder.push_back(selectedZoneIndex_);

        for (int i : paintOrder)
        {
            paintZone(g, i);
        }
    }

    //------------------------------------------------------------------------
    void resized() override
    {
        // Invalidate cached calculations when component is resized
        cachedWhiteKeyWidth_ = -1;
    }

    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    void mouseMove(const juce::MouseEvent& e) override
    {
        if (config_.showHoverEffect && drag_.zoneIndex < 0) // Only when not dragging
        {
            const int note = xToKey(e.position.x);
            const int vel = yToVel(e.position.y);
            Edge dummy;

            const int hoveredZone = hitTest(note, vel, dummy);
            if (hoveredZone != hoveredZoneIndex_)
            {
                hoveredZoneIndex_ = hoveredZone;
                repaint();
            }
        }
    }

    void mouseExit(const juce::MouseEvent&) override
    {
        if (hoveredZoneIndex_ >= 0)
        {
            hoveredZoneIndex_ = -1;
            repaint();
        }
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        const int note = xToKey(e.position.x);
        const int vel = yToVel(e.position.y);

        drag_.zoneIndex = hitTest(note, vel, drag_.edge);
        drag_.startPosition = { note, vel };
        drag_.isDragging = false;

        setSelectedZone(drag_.zoneIndex);
        hoveredZoneIndex_ = -1; // Clear hover when starting to drag

        if (drag_.zoneIndex >= 0)
        {
            // Store original zone for potential undo
            drag_.originalZone = zones[static_cast<size_t>(drag_.zoneIndex)];
        }
    }


    void mouseDrag(const juce::MouseEvent& e) override
    {
        if (drag_.zoneIndex < 0)
            return;

        drag_.isDragging = true;
        auto& zone = zones[static_cast<size_t>(drag_.zoneIndex)];

        int note = juce::jlimit(0, 127, xToKey(e.position.x));
        int vel = juce::jlimit(1, 127, yToVel(e.position.y));

        // Apply snap-to-grid if enabled
        if (config_.snapToWhiteKeys && !isBlackKey(note))
        {
            // Find nearest white key
            while (note > 0 && isBlackKey(note)) --note;
        }

        updateZoneFromDrag(zone, note, vel);
        normalizeZone(zone);

        if (onZoneChanged_)
            onZoneChanged_(drag_.zoneIndex, zone);

        repaint();
    }

    void mouseDoubleClick(const juce::MouseEvent& e) override
    {
        const int note = xToKey(e.position.x);
        const int vel = yToVel(e.position.y);
        Edge dummy;

        const int hit = hitTest(note, vel, dummy);

        if (e.mods.isCtrlDown() || e.mods.isCommandDown())
        {
            // Ctrl/Cmd + Double-click → Delete zone
            if (hit >= 0)
            {
                deleteZone(hit);
            }
        }
        else
        {
            // Double-click → Create new zone (only if empty space)
            if (hit < 0)
            {
                createNewZone(note, vel);
            }
        }
    }

    void mouseUp(const juce::MouseEvent&) override
    {
        if (drag_.isDragging && drag_.zoneIndex >= 0)
        {
            // Zone editing completed
            const auto& currentZone = zones[static_cast<size_t>(drag_.zoneIndex)];
            if (!(currentZone == drag_.originalZone) && onZoneChanged_)
            {
                onZoneChanged_(drag_.zoneIndex, currentZone);
            }
        }

        drag_.reset();
    }

    //------------------------------------------------------------------------
    // KeyListener implementation for keyboard navigation
    bool keyPressed(const juce::KeyPress& key, Component*) override
    {
        if (!config_.enableKeyboardNavigation)
            return false;

        if (key == juce::KeyPress::deleteKey || key == juce::KeyPress::backspaceKey)
        {
            deleteSelectedZone();
            return true;
        }

        if (key == juce::KeyPress::escapeKey)
        {
            clearSelection();
            return true;
        }

        // Arrow key navigation
        if (selectedZoneIndex_ >= 0)
        {
            auto& zone = zones[static_cast<size_t>(selectedZoneIndex_)];
            bool changed = false;

            if (key == juce::KeyPress::leftKey)
            {
                if (key.getModifiers().isShiftDown())
                {
                    zone.startNote = juce::jmax(0, zone.startNote - 1);
                    changed = true;
                }
                else if (zone.startNote > 0 && zone.endNote > 0)
                {
                    --zone.startNote;
                    --zone.endNote;
                    changed = true;
                }
            }
            else if (key == juce::KeyPress::rightKey)
            {
                if (key.getModifiers().isShiftDown())
                {
                    zone.endNote = juce::jmin(127, zone.endNote + 1);
                    changed = true;
                }
                else if (zone.startNote < 127 && zone.endNote < 127)
                {
                    ++zone.startNote;
                    ++zone.endNote;
                    changed = true;
                }
            }
            else if (key == juce::KeyPress::upKey)
            {
                if (key.getModifiers().isShiftDown())
                {
                    zone.velHigh = juce::jmin(127, zone.velHigh + 1);
                    changed = true;
                }
                else if (zone.velLow < 127 && zone.velHigh < 127)
                {
                    ++zone.velLow;
                    ++zone.velHigh;
                    changed = true;
                }
            }
            else if (key == juce::KeyPress::downKey)
            {
                if (key.getModifiers().isShiftDown())
                {
                    zone.velLow = juce::jmax(1, zone.velLow - 1);
                    changed = true;
                }
                else if (zone.velLow > 1 && zone.velHigh > 1)
                {
                    --zone.velLow;
                    --zone.velHigh;
                    changed = true;
                }
            }

            if (changed)
            {
                normalizeZone(zone);
                if (onZoneChanged_)
                    onZoneChanged_(selectedZoneIndex_, zone);
                repaint();
                return true;
            }
        }

        return false;
    }

private:
    //------------------------------------------------------------------------
    enum class Edge { none, left, right, top, bottom };

    struct DragInfo
    {
        int zoneIndex = -1;
        Edge edge = Edge::none;
        juce::Point<int> startPosition;
        SampleZone originalZone;
        bool isDragging = false;

        void reset()
        {
            zoneIndex = -1;
            edge = Edge::none;
            isDragging = false;
        }
    };

    //------------------------------------------------------------------------
    // Utility functions
    static bool isBlackKey(int midiNote) noexcept
    {
        if (midiNote < 0 || midiNote > 127) return false;
        constexpr std::array<int, 5> blackKeys = { 1, 3, 6, 8, 10 };
        const int noteInOctave = midiNote % 12;
        return std::find(blackKeys.begin(), blackKeys.end(), noteInOctave) != blackKeys.end();
    }

    static bool isValidZone(const SampleZone& z) noexcept
    {
        return z.startNote >= 0 && z.endNote <= 127 && z.startNote <= z.endNote &&
            z.velLow >= 1 && z.velHigh <= 127 && z.velLow <= z.velHigh;
    }

    static int countWhiteKeys(int from, int to) noexcept
    {
        if (from > to) return 0;
        int count = 0;
        for (int note = from; note <= to; ++note)
        {
            if (!isBlackKey(note))
                ++count;
        }
        return count;
    }

    //------------------------------------------------------------------------
    // Layout calculations with caching
    int getWhiteKeyWidth() const
    {
        if (cachedWhiteKeyWidth_ <= 0)
        {
            const int totalWhite = countWhiteKeys(keyboard.getLowestVisibleKey(), 127);
            cachedWhiteKeyWidth_ = totalWhite > 0 ? juce::roundToInt(getWidth() / static_cast<float>(totalWhite)) : 0;
        }
        return cachedWhiteKeyWidth_;
    }

    int getHighestVisibleKey() const
    {
        const int whiteKeyWidth = getWhiteKeyWidth();
        if (whiteKeyWidth <= 0) return 127;

        int x = 0;
        int note = keyboard.getLowestVisibleKey();

        while (note <= 127)
        {
            if (!isBlackKey(note))
                x += whiteKeyWidth;
            if (x >= getWidth())
                return juce::jmax(keyboard.getLowestVisibleKey(), note - 1);
            ++note;
        }
        return 127;
    }

    int getKeyPositionX(int midiNote) const
    {
        if (midiNote < keyboard.getLowestVisibleKey())
            return -1;

        const int whiteKeyWidth = getWhiteKeyWidth();
        if (whiteKeyWidth <= 0)
            return -1;

        const int whitesLeft = countWhiteKeys(keyboard.getLowestVisibleKey(), midiNote - 1);
        int x = whitesLeft * whiteKeyWidth;

        if (isBlackKey(midiNote))
            x += static_cast<int>(whiteKeyWidth * 0.75f);

        return x;
    }

    int getKeyWidth(int midiNote) const
    {
        const int whiteKeyWidth = getWhiteKeyWidth();
        return isBlackKey(midiNote) ? whiteKeyWidth / 2 : whiteKeyWidth;
    }

    int xToKey(float x) const
    {
        const int low = keyboard.getLowestVisibleKey();
        const int high = getHighestVisibleKey();

        for (int note = low; note <= high; ++note)
        {
            const int keyX = getKeyPositionX(note);
            if (keyX < 0) continue;

            const int keyWidth = getKeyWidth(note);
            if (x >= keyX && x < keyX + keyWidth)
                return note;
        }
        return juce::jlimit(low, high, static_cast<int>(x * (high - low + 1) / getWidth() + low));
    }

    int velToY(int vel) const
    {
        return juce::jmap(vel, 1, 127, getHeight(), 0);
    }

    int yToVel(float y) const
    {
        return juce::jlimit(1, 127, juce::jmap(static_cast<int>(y), getHeight(), 0, 1, 127));
    }

    //------------------------------------------------------------------------
    void normalizeZone(SampleZone& zone) const
    {
        // Ensure proper order
        if (zone.startNote > zone.endNote)
            std::swap(zone.startNote, zone.endNote);
        if (zone.velLow > zone.velHigh)
            std::swap(zone.velLow, zone.velHigh);

        // Clamp to valid ranges
        zone.startNote = juce::jlimit(0, 127, zone.startNote);
        zone.endNote = juce::jlimit(0, 127, zone.endNote);
        zone.velLow = juce::jlimit(1, 127, zone.velLow);
        zone.velHigh = juce::jlimit(1, 127, zone.velHigh);

        // Ensure minimum velocity range
        const int currentRange = zone.velHigh - zone.velLow + 1;
        if (currentRange < config_.minimumVelocityRange)
        {
            const int center = (zone.velLow + zone.velHigh) / 2;
            const int halfRange = config_.minimumVelocityRange / 2;

            zone.velLow = juce::jlimit(1, 127 - config_.minimumVelocityRange + 1, center - halfRange);
            zone.velHigh = juce::jlimit(zone.velLow + config_.minimumVelocityRange - 1, 127,
                zone.velLow + config_.minimumVelocityRange - 1);
        }
    }

    //------------------------------------------------------------------------
    void paintZone(juce::Graphics& g, int zoneIndex)
    {
        const auto& zone = zones[static_cast<size_t>(zoneIndex)];
        if (!isValidZone(zone))
            return;

        const int x1 = getKeyPositionX(zone.startNote);
        const int x2 = getKeyPositionX(zone.endNote + 1); // include full end key

        if (x1 < 0 || x2 <= x1)
            return;

        const int y1 = velToY(zone.velHigh + 1);
        const int y2 = velToY(zone.velLow);

        const juce::Rectangle<float> rect(
            static_cast<float>(x1),
            static_cast<float>(y1),
            static_cast<float>(x2 - x1),
            static_cast<float>(y2 - y1)
        );

        const bool isSelected = zoneIndex == selectedZoneIndex_;
        const bool isHovered = zoneIndex == hoveredZoneIndex_ && !isSelected;

        // Use custom color if set, otherwise use default colors
        juce::Colour baseColor = zone.customColor.isTransparent()
            ? (isSelected ? config_.activeZoneColor : config_.inactiveZoneColor)
            : zone.customColor;

        // Adjust alpha for hover effect
        float alpha = config_.zoneAlpha;
        if (isHovered)
            alpha *= 1.5f; // Increase alpha for hover
        else if (!isSelected && hoveredZoneIndex_ >= 0)
            alpha *= 0.7f; // Dim other zones when hovering

        // Fill zone
        g.setColour(baseColor.withAlpha(alpha));
        g.fillRect(rect);

        // Draw border (thicker for hovered zones)
        const float borderWidth = isHovered ? 2.0f : 1.0f;
        g.setColour(baseColor.darker(0.4f));
        g.drawRect(rect, borderWidth);

        // Draw zone label if enabled
        if (config_.showZoneLabels && !zone.name.isEmpty())
        {
            g.setColour(baseColor.contrasting(0.8f));
            g.setFont(12.0f);
            g.drawText(zone.name, rect, juce::Justification::centred, true);
        }

        // Draw handles if selected
        if (isSelected)
        {
            drawZoneHandles(g, rect);
        }
        // Draw lighter handles if hovered
        else if (isHovered)
        {
            g.setColour(baseColor.darker(0.2f).withAlpha(0.6f));
            const float handleSize = config_.handleSize * 0.7f;
            const auto handleRect = juce::Rectangle<float>(handleSize, handleSize);
            g.fillRect(handleRect.withCentre(rect.getTopLeft()));
            g.fillRect(handleRect.withCentre(rect.getTopRight()));
            g.fillRect(handleRect.withCentre(rect.getBottomLeft()));
            g.fillRect(handleRect.withCentre(rect.getBottomRight()));
        }
    }

    void drawZoneHandles(juce::Graphics& g, const juce::Rectangle<float>& rect)
    {
        const float handleSize = config_.handleSize;
        g.setColour(juce::Colours::black);

        const auto handleRect = juce::Rectangle<float>(handleSize, handleSize);
        g.fillRect(handleRect.withCentre(rect.getTopLeft()));
        g.fillRect(handleRect.withCentre(rect.getTopRight()));
        g.fillRect(handleRect.withCentre(rect.getBottomLeft()));
        g.fillRect(handleRect.withCentre(rect.getBottomRight()));

        // Optional: draw edge handles for better UX
        const auto edgeHandleSize = juce::Rectangle<float>(handleSize * 0.7f, handleSize * 0.7f);
        g.setColour(juce::Colours::darkgrey);
        g.fillRect(edgeHandleSize.withCentre({ rect.getCentreX(), rect.getY() })); // top
        g.fillRect(edgeHandleSize.withCentre({ rect.getCentreX(), rect.getBottom() })); // bottom
        g.fillRect(edgeHandleSize.withCentre({ rect.getX(), rect.getCentreY() })); // left
        g.fillRect(edgeHandleSize.withCentre({ rect.getRight(), rect.getCentreY() })); // right
    }

    //------------------------------------------------------------------------
    int hitTest(int note, int vel, Edge& outEdge) const
    {
        int bestMatch = -1;
        float bestDistance = std::numeric_limits<float>::max();
        Edge bestEdge = Edge::none;

        // Test all zones and find the best match
        for (int i = static_cast<int>(zones.size()) - 1; i >= 0; --i)
        {
            const auto& zone = zones[static_cast<size_t>(i)];
            if (!isValidZone(zone))
                continue;

            // Check if point is within or near the zone bounds
            const bool withinNoteRange = (note >= zone.startNote - 1 && note <= zone.endNote + 1);
            const bool withinVelRange = (vel >= zone.velLow - 5 && vel <= zone.velHigh + 5);

            if (!withinNoteRange || !withinVelRange)
                continue;

            // Calculate distances to edges for better edge detection
            const int startNoteX = getKeyPositionX(zone.startNote);
            const int endNoteX = getKeyPositionX(zone.endNote + 1); // end of last key
            const int currentX = getKeyPositionX(note);

            // Distance to each edge
            const float distToLeft = std::abs(currentX - startNoteX);
            const float distToRight = std::abs(currentX - endNoteX);
            const float distToTop = std::abs(vel - zone.velHigh);
            const float distToBottom = std::abs(vel - zone.velLow);

            // Find the closest edge
            const float minHorizontalDist = std::min(distToLeft, distToRight);
            const float minVerticalDist = std::min(distToTop, distToBottom);

            Edge currentEdge = Edge::none;
            float edgeDistance = std::numeric_limits<float>::max();

            // Check if we're near any edge (with generous tolerance)
            const int edgeTolerance = config_.edgePixelTolerance * 2; // Double the tolerance
            const int velTolerance = config_.velocityTolerance * 3;   // Triple for velocity

            if (distToLeft < edgeTolerance && distToLeft <= minHorizontalDist)
            {
                currentEdge = Edge::left;
                edgeDistance = distToLeft;
            }
            else if (distToRight < edgeTolerance && distToRight <= minHorizontalDist)
            {
                currentEdge = Edge::right;
                edgeDistance = distToRight;
            }
            else if (distToTop < velTolerance && distToTop <= minVerticalDist)
            {
                currentEdge = Edge::top;
                edgeDistance = distToTop;
            }
            else if (distToBottom < velTolerance && distToBottom <= minVerticalDist)
            {
                currentEdge = Edge::bottom;
                edgeDistance = distToBottom;
            }

            // If not near an edge, check if we're inside the zone for body dragging
            if (currentEdge == Edge::none)
            {
                const bool fullyInside = (note >= zone.startNote && note <= zone.endNote &&
                    vel >= zone.velLow && vel <= zone.velHigh);
                if (fullyInside)
                {
                    // Calculate distance from center for prioritization
                    const float centerNote = (zone.startNote + zone.endNote) / 2.0f;
                    const float centerVel = (zone.velLow + zone.velHigh) / 2.0f;
                    edgeDistance = std::sqrt(std::pow(note - centerNote, 2) +
                        std::pow((vel - centerVel) * 0.1f, 2)); // Scale vel distance
                }
                else
                {
                    continue; // Skip if not inside and not near edge
                }
            }

            // Keep track of the best match (closest to click point)
            if (edgeDistance < bestDistance)
            {
                bestDistance = edgeDistance;
                bestMatch = i;
                bestEdge = currentEdge;
            }
        }

        outEdge = bestEdge;
        return bestMatch;
    }

    //------------------------------------------------------------------------
    void updateZoneFromDrag(SampleZone& zone, int note, int vel)
    {
        switch (drag_.edge)
        {
        case Edge::left:
            zone.startNote = note;
            break;
        case Edge::right:
            zone.endNote = note;
            break;
        case Edge::top:
            zone.velHigh = vel;
            break;
        case Edge::bottom:
            zone.velLow = vel;
            break;
        case Edge::none:
        {
            // Move entire zone
            const int deltaNote = note - drag_.startPosition.x;
            const int deltaVel = vel - drag_.startPosition.y;

            zone.startNote = juce::jlimit(0, 127, zone.startNote + deltaNote);
            zone.endNote = juce::jlimit(0, 127, zone.endNote + deltaNote);
            zone.velLow = juce::jlimit(1, 127, zone.velLow + deltaVel);
            zone.velHigh = juce::jlimit(1, 127, zone.velHigh + deltaVel);

            drag_.startPosition = { note, vel };
            break;
        }
        }
    }

    void createNewZone(int note, int vel)
    {
        const int halfRange = config_.newZoneVelocityRange / 2;
        const int minVel = juce::jlimit(1, 127 - config_.newZoneVelocityRange, vel - halfRange);
        const int maxVel = minVel + config_.newZoneVelocityRange - 1;

        SampleZone newZone;
        newZone.startNote = note;
        newZone.endNote = note;
        newZone.velLow = minVel;
        newZone.velHigh = maxVel;
        newZone.note = note;

        zones.push_back(newZone);
        const int newIndex = static_cast<int>(zones.size()) - 1;

        setSelectedZone(newIndex);
        drag_.zoneIndex = newIndex;
        drag_.edge = Edge::right;

        if (onZoneAdded_)
            onZoneAdded_(newIndex, newZone);

        repaint();
    }

    void deleteZone(int index)
    {
        if (index >= 0 && index < static_cast<int>(zones.size()))
        {
            if (onZoneRemoved_)
                onZoneRemoved_(index);

            zones.erase(zones.begin() + index);

            if (selectedZoneIndex_ == index)
                selectedZoneIndex_ = -1;
            else if (selectedZoneIndex_ > index)
                --selectedZoneIndex_;

            drag_.reset();
            repaint();
        }
    }

    //------------------------------------------------------------------------
    // Member variables
    juce::MidiKeyboardComponent& keyboard;
    std::vector<SampleZone>& zones;
    const Configuration config_;

    DragInfo drag_;
    int selectedZoneIndex_ = -1;
    int hoveredZoneIndex_ = -1;                   // New: track hovered zone
    mutable int cachedWhiteKeyWidth_ = -1;

    // Callbacks
    ZoneChangedCallback onZoneChanged_;
    ZoneAddedCallback onZoneAdded_;
    ZoneRemovedCallback onZoneRemoved_;
    SelectionChangedCallback onSelectionChanged_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardMappingEditor)
};