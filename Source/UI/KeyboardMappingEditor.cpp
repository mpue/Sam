/*
    KeyboardMappingEditor.cpp (Extended with Drop Functionality)
    ---------------------------------------------------------------------------
    Implementation of the extended KeyboardMappingEditor with drag and drop
    support for audio files from ExtendedFileBrowser.

    Author: Matthias Püski / Extended by Claude
*/

#include "KeyboardMappingEditor.h"

//============================================================================
// Zone lookup methods implementation
//============================================================================


//============================================================================
// Component overrides
//============================================================================
void KeyboardMappingEditor::resized()
{
    cachedWhiteKeyWidth_ = -1;
}

void KeyboardMappingEditor::mouseMove(const juce::MouseEvent& e)
{
    if (config_.showHoverEffect && drag_.zoneIndex < 0)
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

void KeyboardMappingEditor::mouseExit(const juce::MouseEvent&)
{
    if (hoveredZoneIndex_ >= 0)
    {
        hoveredZoneIndex_ = -1;
        repaint();
    }
}

void KeyboardMappingEditor::mouseDown(const juce::MouseEvent& e)
{
    const int note = xToKey(e.position.x);
    const int vel = yToVel(e.position.y);

    drag_.zoneIndex = hitTest(note, vel, drag_.edge);
    drag_.startPosition = { note, vel };
    drag_.isDragging = false;

    setSelectedZone(drag_.zoneIndex);
    hoveredZoneIndex_ = -1;

    if (drag_.zoneIndex >= 0)
    {
        drag_.originalZone = zones[static_cast<size_t>(drag_.zoneIndex)];
    }
}

void KeyboardMappingEditor::mouseDrag(const juce::MouseEvent& e)
{
    if (drag_.zoneIndex < 0)
        return;

    drag_.isDragging = true;
    auto& zone = zones[static_cast<size_t>(drag_.zoneIndex)];

    int note = juce::jlimit(0, 127, xToKey(e.position.x));
    int vel = juce::jlimit(1, 127, yToVel(e.position.y));

    if (config_.snapToWhiteKeys && !isBlackKey(note))
    {
        while (note > 0 && isBlackKey(note)) --note;
    }

    updateZoneFromDrag(zone, note, vel);
    normalizeZone(zone);

    if (onZoneChanged_)
        onZoneChanged_(drag_.zoneIndex, zone);

    repaint();
}

void KeyboardMappingEditor::mouseDoubleClick(const juce::MouseEvent& e)
{
    const int note = xToKey(e.position.x);
    const int vel = yToVel(e.position.y);
    Edge dummy;

    const int hit = hitTest(note, vel, dummy);

    if (e.mods.isCtrlDown() || e.mods.isCommandDown())
    {
        if (hit >= 0)
        {
            deleteZone(hit);
        }
    }
    else
    {
        if (hit < 0)
        {
            createNewZone(note, vel);
        }
    }
}

void KeyboardMappingEditor::mouseUp(const juce::MouseEvent&)
{
    if (drag_.isDragging && drag_.zoneIndex >= 0)
    {
        const auto& currentZone = zones[static_cast<size_t>(drag_.zoneIndex)];
        if (!(currentZone == drag_.originalZone) && onZoneChanged_)
        {
            onZoneChanged_(drag_.zoneIndex, currentZone);
        }
    }

    drag_.reset();
}

//============================================================================
// KeyListener implementation
//============================================================================
bool KeyboardMappingEditor::keyPressed(const juce::KeyPress& key, Component*)
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

//============================================================================
// Utility functions (static)
//============================================================================
bool KeyboardMappingEditor::isBlackKey(int midiNote) noexcept
{
    if (midiNote < 0 || midiNote > 127) return false;
    constexpr std::array<int, 5> blackKeys = { 1, 3, 6, 8, 10 };
    const int noteInOctave = midiNote % 12;
    return std::find(blackKeys.begin(), blackKeys.end(), noteInOctave) != blackKeys.end();
}

bool KeyboardMappingEditor::isValidZone(const SampleZone& z) noexcept
{
    return z.startNote >= 0 && z.endNote <= 127 && z.startNote <= z.endNote &&
        z.velLow >= 1 && z.velHigh <= 127 && z.velLow <= z.velHigh;
}


int KeyboardMappingEditor::countWhiteKeys(int from, int to) noexcept
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

//============================================================================
// Layout calculations
//============================================================================
int KeyboardMappingEditor::getWhiteKeyWidth() const
{
    if (cachedWhiteKeyWidth_ <= 0)
    {
        const int totalWhite = countWhiteKeys(keyboard.getLowestVisibleKey(), 127);
        cachedWhiteKeyWidth_ = totalWhite > 0 ? juce::roundToInt(getWidth() / static_cast<float>(totalWhite)) : 0;
    }
    return cachedWhiteKeyWidth_;
}

int KeyboardMappingEditor::getHighestVisibleKey() const
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

int KeyboardMappingEditor::getKeyPositionX(int midiNote) const
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

int KeyboardMappingEditor::getKeyWidth(int midiNote) const
{
    const int whiteKeyWidth = getWhiteKeyWidth();
    return isBlackKey(midiNote) ? whiteKeyWidth / 2 : whiteKeyWidth;
}

int KeyboardMappingEditor::xToKey(float x) const
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

int KeyboardMappingEditor::velToY(int vel) const
{
    return juce::jmap(vel, 1, 127, getHeight(), 0);
}

int KeyboardMappingEditor::yToVel(float y) const
{
    return juce::jlimit(1, 127, juce::jmap(static_cast<int>(y), getHeight(), 0, 1, 127));
}

//============================================================================
// Zone management
//============================================================================
void KeyboardMappingEditor::normalizeZone(SampleZone& zone) const
{
    if (zone.startNote > zone.endNote)
        std::swap(zone.startNote, zone.endNote);
    if (zone.velLow > zone.velHigh)
        std::swap(zone.velLow, zone.velHigh);

    zone.startNote = juce::jlimit(0, 127, zone.startNote);
    zone.endNote = juce::jlimit(0, 127, zone.endNote);
    zone.velLow = juce::jlimit(1, 127, zone.velLow);
    zone.velHigh = juce::jlimit(1, 127, zone.velHigh);

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

void KeyboardMappingEditor::paintZone(juce::Graphics& g, int zoneIndex)
{
    const auto& zone = zones[static_cast<size_t>(zoneIndex)];
    if (!isValidZone(zone))
        return;

    const int x1 = getKeyPositionX(zone.startNote);
    const int x2 = getKeyPositionX(zone.endNote + 1);

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

    juce::Colour baseColor = zone.customColor.isTransparent()
        ? (isSelected ? config_.activeZoneColor : config_.inactiveZoneColor)
        : zone.customColor;

    float alpha = config_.zoneAlpha;
    if (isHovered)
        alpha *= 1.5f;
    else if (!isSelected && hoveredZoneIndex_ >= 0)
        alpha *= 0.7f;

    g.setColour(baseColor.withAlpha(alpha));
    g.fillRect(rect);

    const float borderWidth = isHovered ? 2.0f : 1.0f;
    g.setColour(baseColor.darker(0.4f));
    g.drawRect(rect, borderWidth);

    if (config_.showZoneLabels && !zone.name.isEmpty())
    {
        g.setColour(baseColor.contrasting(0.8f));
        g.setFont(12.0f);
        g.drawText(zone.name, rect, juce::Justification::centred, true);
    }

    if (isSelected)
    {
        drawZoneHandles(g, rect);
    }
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

void KeyboardMappingEditor::drawZoneHandles(juce::Graphics& g, const juce::Rectangle<float>& rect)
{
    const float handleSize = config_.handleSize;
    g.setColour(juce::Colours::black);

    const auto handleRect = juce::Rectangle<float>(handleSize, handleSize);
    g.fillRect(handleRect.withCentre(rect.getTopLeft()));
    g.fillRect(handleRect.withCentre(rect.getTopRight()));
    g.fillRect(handleRect.withCentre(rect.getBottomLeft()));
    g.fillRect(handleRect.withCentre(rect.getBottomRight()));

    const auto edgeHandleSize = juce::Rectangle<float>(handleSize * 0.7f, handleSize * 0.7f);
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(edgeHandleSize.withCentre({ rect.getCentreX(), rect.getY() }));
    g.fillRect(edgeHandleSize.withCentre({ rect.getCentreX(), rect.getBottom() }));
    g.fillRect(edgeHandleSize.withCentre({ rect.getX(), rect.getCentreY() }));
    g.fillRect(edgeHandleSize.withCentre({ rect.getRight(), rect.getCentreY() }));
}

int KeyboardMappingEditor::hitTest(int note, int vel, Edge& outEdge) const
{
    int bestMatch = -1;
    float bestDistance = std::numeric_limits<float>::max();
    Edge bestEdge = Edge::none;

    for (int i = static_cast<int>(zones.size()) - 1; i >= 0; --i)
    {
        const auto& zone = zones[static_cast<size_t>(i)];
        if (!isValidZone(zone))
            continue;

        const bool withinNoteRange = (note >= zone.startNote - 1 && note <= zone.endNote + 1);
        const bool withinVelRange = (vel >= zone.velLow - 5 && vel <= zone.velHigh + 5);

        if (!withinNoteRange || !withinVelRange)
            continue;

        const int startNoteX = getKeyPositionX(zone.startNote);
        const int endNoteX = getKeyPositionX(zone.endNote + 1);
        const int currentX = getKeyPositionX(note);

        const float distToLeft = std::abs(currentX - startNoteX);
        const float distToRight = std::abs(currentX - endNoteX);
        const float distToTop = std::abs(vel - zone.velHigh);
        const float distToBottom = std::abs(vel - zone.velLow);

        const float minHorizontalDist = std::min(distToLeft, distToRight);
        const float minVerticalDist = std::min(distToTop, distToBottom);

        Edge currentEdge = Edge::none;
        float edgeDistance = std::numeric_limits<float>::max();

        const int edgeTolerance = config_.edgePixelTolerance * 2;
        const int velTolerance = config_.velocityTolerance * 3;

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

        if (currentEdge == Edge::none)
        {
            const bool fullyInside = (note >= zone.startNote && note <= zone.endNote &&
                vel >= zone.velLow && vel <= zone.velHigh);
            if (fullyInside)
            {
                const float centerNote = (zone.startNote + zone.endNote) / 2.0f;
                const float centerVel = (zone.velLow + zone.velHigh) / 2.0f;
                edgeDistance = std::sqrt(std::pow(note - centerNote, 2) +
                    std::pow((vel - centerVel) * 0.1f, 2));
            }
            else
            {
                continue;
            }
        }

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

void KeyboardMappingEditor::updateZoneFromDrag(SampleZone& zone, int note, int vel)
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

void KeyboardMappingEditor::createNewZone(int note, int vel)
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

void KeyboardMappingEditor::deleteZone(int index)
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