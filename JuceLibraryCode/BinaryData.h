/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   carbon_jpg;
    const int            carbon_jpgSize = 472610;

    extern const char*   Fader_png;
    const int            Fader_pngSize = 19460;

    extern const char*   Knob_32_png;
    const int            Knob_32_pngSize = 186292;

    extern const char*   Knob_48_png;
    const int            Knob_48_pngSize = 413882;

    extern const char*   Knob_64_png;
    const int            Knob_64_pngSize = 495933;

    extern const char*   Knob_128_png;
    const int            Knob_128_pngSize = 2713110;

    extern const char*   knob_shadow_png;
    const int            knob_shadow_pngSize = 5641;

    extern const char*   knob_shadow_64_png;
    const int            knob_shadow_64_pngSize = 3236;

    extern const char*   oscillator_noise_48_png;
    const int            oscillator_noise_48_pngSize = 17465;

    extern const char*   oscillator_saw_48_png;
    const int            oscillator_saw_48_pngSize = 15735;

    extern const char*   oscillator_sine_48_png;
    const int            oscillator_sine_48_pngSize = 18394;

    extern const char*   oscillator_square_48_png;
    const int            oscillator_square_48_pngSize = 17545;

    extern const char*   sam_png;
    const int            sam_pngSize = 19594;

    extern const char*   sam2_png;
    const int            sam2_pngSize = 1599347;

    extern const char*   sam2_mappings_png;
    const int            sam2_mappings_pngSize = 836279;

    extern const char*   vintage_vu_png;
    const int            vintage_vu_pngSize = 30329;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 16;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
