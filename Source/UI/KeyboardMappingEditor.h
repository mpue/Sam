/*
    KeyboardMappingEditor.h (Extended with Drop Functionality)
    ---------------------------------------------------------------------------
    A rock‑solid overlay component for JUCE that lets the user draw & edit sample
    zones directly on top of a juce::MidiKeyboardComponent.

    NEW FEATURES:
    - Accepts drops from ExtendedFileBrowser
    - Shows preview SampleZone while dragging
    - Creates SampleZone with full velocity range on drop
    - Preview zone follows mouse during drag operation

    Author: Matthias Püski / Extended by Claude
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
    juce::File audioFile;   // NEW: Store the audio file for this zone

    // std::unique_ptr requires special handling
    std::unique_ptr<Sampler> sampler = nullptr;

    // Default constructor
    SampleZone() = default;

    // Copy constructor - creates new sampler if source has one
    SampleZone(const SampleZone& other)
        : startNote(other.startNote), endNote(other.endNote)
        , velLow(other.velLow), velHigh(other.velHigh), note(other.note)
        , name(other.name), customColor(other.customColor)
        , audioFile(other.audioFile)  // Copy the file reference
        , sampler(nullptr) // Don't copy the sampler - create new one if needed
    {
    }

    // Copy assignment operator
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
            audioFile = other.audioFile;  // Copy the file reference
            sampler.reset();
        }
        return *this;
    }

    // Move constructor
    SampleZone(SampleZone&& other) noexcept
        : startNote(other.startNote), endNote(other.endNote)
        , velLow(other.velLow), velHigh(other.velHigh), note(other.note)
        , name(std::move(other.name)), customColor(other.customColor)
        , audioFile(std::move(other.audioFile))  // Move the file reference
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
            audioFile = std::move(other.audioFile);  // Move the file reference
            sampler = std::move(other.sampler);  // Move the unique_ptr
        }
        return *this;
    }

    bool operator==(const SampleZone& other) const noexcept
    {
        return startNote == other.startNote && endNote == other.endNote &&
            velLow == other.velLow && velHigh == other.velHigh && note == other.note;
    }

    bool hasSampler() const noexcept
    {
        return sampler != nullptr;
    }

    void clearSampler()
    {
        sampler.reset();
    }
};

//============================================================================
class KeyboardMappingEditor : public juce::Component,
    public juce::KeyListener,
    public juce::DragAndDropTarget  // NEW: Add DragAndDropTarget
{
public:
    //------------------------------------------------------------------------
    // Callback types for zone changes
    using ZoneChangedCallback = std::function<void(int zoneIndex, const SampleZone& zone)>;
    using ZoneAddedCallback = std::function<void(int zoneIndex, const SampleZone& zone)>;
    using ZoneRemovedCallback = std::function<void(int zoneIndex)>;
    using SelectionChangedCallback = std::function<void(int zoneIndex)>;
    using FileDroppedCallback = std::function<void(const juce::File& file, int targetNote)>;  // NEW

    //------------------------------------------------------------------------
    // Configuration struct for better customization
    struct Configuration
    {
        juce::Colour activeZoneColor = juce::Colours::orange;
        juce::Colour inactiveZoneColor = juce::Colours::skyblue;
        juce::Colour previewZoneColor = juce::Colours::yellow;  // NEW: Preview color
        float zoneAlpha = 0.35f;
        float previewZoneAlpha = 0.6f;                          // NEW: Preview alpha
        float handleSize = 8.0f;
        int edgePixelTolerance = 8;
        int velocityTolerance = 5;
        int minimumVelocityRange = 10;
        int newZoneVelocityRange = 10;
        int defaultZoneWidth = 1;                               // NEW: Default width in semitones
        bool snapToWhiteKeys = false;
        bool showZoneLabels = true;
        bool enableKeyboardNavigation = true;
        bool showHoverEffect = true;
        bool enableFileDrop = true;                             // NEW: Enable/disable file drops
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
    // NEW: DragAndDropTarget implementation
    bool isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override
    {
        if (!config_.enableFileDrop)
            return false;

        // Check if the drag source contains a file path (from ExtendedFileBrowser)
        juce::String dragDescription = dragSourceDetails.description.toString();
        if (dragDescription.isEmpty())
            return false;

        // Check if it's a valid audio file
        juce::File file(dragDescription);
        return file.exists() && isAudioFile(file);
    }

    void itemDragEnter(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override
    {
        if (!isInterestedInDragSource(dragSourceDetails))
            return;

        dropState_.isDragOver = true;
        dropState_.draggedFile = juce::File(dragSourceDetails.description.toString());

        // Update preview zone position
        updatePreviewZone(dragSourceDetails.localPosition);
        repaint();
    }

    void itemDragMove(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override
    {
        if (!dropState_.isDragOver)
            return;

        // Update preview zone to follow mouse
        updatePreviewZone(dragSourceDetails.localPosition);
        repaint();
    }

    void itemDragExit(const juce::DragAndDropTarget::SourceDetails&) override
    {
        dropState_.reset();
        repaint();
    }

    void itemDropped(const juce::DragAndDropTarget::SourceDetails& dragSourceDetails) override
    {
        if (!dropState_.isDragOver)
            return;

        const juce::File droppedFile = dropState_.draggedFile;
        const int targetNote = dropState_.previewZone.startNote;

        // Create new SampleZone
        createSampleZoneFromFile(droppedFile, targetNote);

        // Notify callback if set
        if (onFileDropped_)
            onFileDropped_(droppedFile, targetNote);

        dropState_.reset();
        repaint();
    }

    //------------------------------------------------------------------------
    // Callback setters
    void setZoneChangedCallback(ZoneChangedCallback callback) { onZoneChanged_ = std::move(callback); }
    void setZoneAddedCallback(ZoneAddedCallback callback) { onZoneAdded_ = std::move(callback); }
    void setZoneRemovedCallback(ZoneRemovedCallback callback) { onZoneRemoved_ = std::move(callback); }
    void setSelectionChangedCallback(SelectionChangedCallback callback) { onSelectionChanged_ = std::move(callback); }
    void setFileDroppedCallback(FileDroppedCallback callback) { onFileDropped_ = std::move(callback); }  // NEW

    //------------------------------------------------------------------------
    // Public API (unchanged from original)
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
    void clearSelection() { setSelectedZone(-1); }
    void deleteSelectedZone()
    {
        if (selectedZoneIndex_ >= 0 && selectedZoneIndex_ < static_cast<int>(zones.size()))
        {
            deleteZone(selectedZoneIndex_);
        }
    }

    // Zone lookup methods (unchanged from original)
    int findZoneForNote(int midiNote) const;
    int findZoneForNoteAndVelocity(int midiNote, int velocity) const;
    std::vector<int> findAllZonesForNote(int midiNote) const;
    std::vector<int> findAllZonesForNoteAndVelocity(int midiNote, int velocity) const;
    int findBestZoneForNoteAndVelocity(int midiNote, int velocity) const;
    const SampleZone* getZone(int index) const;
    SampleZone* getZone(int index);
    int getNumZones() const noexcept;

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

        // NEW: Paint preview zone on top
        if (dropState_.isDragOver)
        {
            paintPreviewZone(g);
        }
    }

    void resized() override;
    void mouseMove(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent&) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseDoubleClick(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent&) override;
    bool keyPressed(const juce::KeyPress& key, Component*) override;

private:
    //------------------------------------------------------------------------
    // NEW: Drop state management
    struct DropState
    {
        bool isDragOver = false;
        juce::File draggedFile;

        struct PreviewZone
        {
            int startNote = 60;
            int endNote = 60;
            int velLow = 1;
            int velHigh = 127;
        } previewZone;

        void reset()
        {
            isDragOver = false;
            draggedFile = juce::File();
        }
    } dropState_;

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
    // NEW: Helper methods for file drop functionality
    bool isAudioFile(const juce::File& file) const
    {
        const juce::String ext = file.getFileExtension().toLowerCase();
        return ext == ".wav" || ext == ".aif" || ext == ".aiff" ||
            ext == ".mp3" || ext == ".ogg" || ext == ".flac" || ext == ".sam";
    }

    void updatePreviewZone(const juce::Point<int>& position)
    {
        const int targetNote = xToKey(static_cast<float>(position.x));
        const int halfWidth = config_.defaultZoneWidth / 2;

        dropState_.previewZone.startNote = juce::jlimit(0, 127, targetNote - halfWidth);
        dropState_.previewZone.endNote = juce::jlimit(0, 127, targetNote + halfWidth);
        dropState_.previewZone.velLow = 1;
        dropState_.previewZone.velHigh = 127;

        // Snap to white keys if enabled
        if (config_.snapToWhiteKeys)
        {
            while (dropState_.previewZone.startNote > 0 && isBlackKey(dropState_.previewZone.startNote))
                --dropState_.previewZone.startNote;
            while (dropState_.previewZone.endNote < 127 && isBlackKey(dropState_.previewZone.endNote))
                ++dropState_.previewZone.endNote;
        }
    }

    void paintPreviewZone(juce::Graphics& g)
    {
        const auto& preview = dropState_.previewZone;

        const int x1 = getKeyPositionX(preview.startNote);
        const int x2 = getKeyPositionX(preview.endNote + 1);

        if (x1 < 0 || x2 <= x1)
            return;

        const int y1 = velToY(preview.velHigh + 1);
        const int y2 = velToY(preview.velLow);

        const juce::Rectangle<float> rect(
            static_cast<float>(x1),
            static_cast<float>(y1),
            static_cast<float>(x2 - x1),
            static_cast<float>(y2 - y1)
        );

        // Draw preview zone with special styling
        g.setColour(config_.previewZoneColor.withAlpha(config_.previewZoneAlpha));
        g.fillRect(rect);

        // Draw dashed border
        g.setColour(config_.previewZoneColor.darker(0.3f));
        const float dashLengths[] = { 5.0f, 3.0f };
        g.drawDashedLine(juce::Line<float>(rect.getTopLeft(), rect.getTopRight()), dashLengths, 2);
        g.drawDashedLine(juce::Line<float>(rect.getTopRight(), rect.getBottomRight()), dashLengths, 2);
        g.drawDashedLine(juce::Line<float>(rect.getBottomRight(), rect.getBottomLeft()), dashLengths, 2);
        g.drawDashedLine(juce::Line<float>(rect.getBottomLeft(), rect.getTopLeft()), dashLengths, 2);

        // Draw file name
        if (dropState_.draggedFile.exists())
        {
            g.setColour(config_.previewZoneColor.contrasting(0.9f));
            g.setFont(11.0f);
            const juce::String fileName = dropState_.draggedFile.getFileNameWithoutExtension();
            g.drawText(fileName, rect, juce::Justification::centred, true);
        }
    }

    void createSampleZoneFromFile(const juce::File& audioFile, int targetNote)
    {
        SampleZone newZone;
        const int halfWidth = config_.defaultZoneWidth / 2;

        newZone.startNote = juce::jlimit(0, 127, targetNote - halfWidth);
        newZone.endNote = juce::jlimit(0, 127, targetNote + halfWidth);
        newZone.velLow = 1;
        newZone.velHigh = 127;
        newZone.note = targetNote;
        newZone.audioFile = audioFile;
        newZone.name = audioFile.getFileNameWithoutExtension();

        // Create and load sampler
 /*       newZone.sampler = std::make_unique<Sampler>();
        if (newZone.sampler)
        {
            newZone.sampler->loadSample(audioFile);
        }*/

        zones.push_back(newZone);
        const int newIndex = static_cast<int>(zones.size()) - 1;

        setSelectedZone(newIndex);

        if (onZoneAdded_)
            onZoneAdded_(newIndex, newZone);
    }

    //------------------------------------------------------------------------
    // Utility functions and other private methods (unchanged from original)
    static bool isBlackKey(int midiNote) noexcept;
    static bool isValidZone(const SampleZone& z) noexcept;
    static int countWhiteKeys(int from, int to) noexcept;

    int getWhiteKeyWidth() const;
    int getHighestVisibleKey() const;
    int getKeyPositionX(int midiNote) const;
    int getKeyWidth(int midiNote) const;
    int xToKey(float x) const;
    int velToY(int vel) const;
    int yToVel(float y) const;

    void normalizeZone(SampleZone& zone) const;
    void paintZone(juce::Graphics& g, int zoneIndex);
    void drawZoneHandles(juce::Graphics& g, const juce::Rectangle<float>& rect);
    int hitTest(int note, int vel, Edge& outEdge) const;
    void updateZoneFromDrag(SampleZone& zone, int note, int vel);
    void createNewZone(int note, int vel);
    void deleteZone(int index);

    //------------------------------------------------------------------------
    // Member variables
    juce::MidiKeyboardComponent& keyboard;
    std::vector<SampleZone>& zones;
    const Configuration config_;

    DragInfo drag_;
    int selectedZoneIndex_ = -1;
    int hoveredZoneIndex_ = -1;
    mutable int cachedWhiteKeyWidth_ = -1;

    // Callbacks
    ZoneChangedCallback onZoneChanged_;
    ZoneAddedCallback onZoneAdded_;
    ZoneRemovedCallback onZoneRemoved_;
    SelectionChangedCallback onSelectionChanged_;
    FileDroppedCallback onFileDropped_;  // NEW

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardMappingEditor)
};