#include "../JuceLibraryCode/JuceHeader.h"

class AudioRecorder
{
public:
    void startRecording(const juce::File& file, double sampleRate, int numChannels)
    {
        stopRecording();

        if (file.existsAsFile())
            file.deleteFile();

        outputStream = file.createOutputStream();

        if (outputStream != nullptr)
        {
            writer.reset(wavFormat.createWriterFor(outputStream.get(), sampleRate, numChannels, 16, {}, 0));
            outputStream.release(); // ownership transferred to writer
        }
    }

    void stopRecording()
    {
        writer = nullptr;
    }

    void writeAudioData(const juce::AudioBuffer<float>& buffer)
    {
        if (writer != nullptr)
            writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
    }

private:
    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer;
    std::unique_ptr<juce::FileOutputStream> outputStream;
};
