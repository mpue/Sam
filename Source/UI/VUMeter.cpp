#include <JuceHeader.h>
#include "VUMeter.h"
#define PI 3.14159265f

VUMeter::VUMeter()
{
    setOpaque(false);
    startTimerHz(30); // 30 FPS für flüssige Bewegung
}

VUMeter::~VUMeter()
{
    stopTimer();
}

void VUMeter::paint(juce::Graphics& g)
{
    float angle = 210.0f + (90.0f * currentMagnitude);
    float radius = 50.0f;
    float centerX = getWidth() / 2.0f;
    float centerY = getHeight() / 2.0f + 10.0f;

    float x = centerX + std::cos(angle * PI / 180.0f) * radius;
    float y = centerY + std::sin(angle * PI / 180.0f) * radius;

    juce::Line<float> line(centerX, centerY, x, y);

    g.drawImageAt(juce::ImageCache::getFromMemory(BinaryData::vintage_vu_png, BinaryData::vintage_vu_pngSize), 0, 0);
    g.drawArrow(line, 3, 6, 6);
}

void VUMeter::resized()
{
}

void VUMeter::setMagnitude(float newMagnitude)
{
    targetMagnitude = juce::jlimit(0.0f, 1.0f, newMagnitude); // Sicherheitsmaßnahme
}

void VUMeter::timerCallback()
{
    // Sanftes Annähern mit einfacher Interpolation
    const float smoothingFactor = 0.1f;
    currentMagnitude += (targetMagnitude - currentMagnitude) * smoothingFactor;

    repaint();
}
