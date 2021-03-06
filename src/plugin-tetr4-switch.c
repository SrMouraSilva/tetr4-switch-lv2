#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2-hmi.h"

#include "model/tetr4-switch.h"
#include "extension/lv2-hmi-extension.h"
#include "extension/atom-extension.h"


typedef enum {
    // Outputs
    OUTPUT_CV_1 = 0,
    OUTPUT_CV_2 = 1,
    OUTPUT_CV_3 = 2,
    OUTPUT_CV_4 = 3,

    // Preset selectors (footswitch selectors)
    PRESET_SELECTOR_1 = 4,
    PRESET_SELECTOR_2 = 5,
    PRESET_SELECTOR_3 = 6,
    PRESET_SELECTOR_4 = 7,

    // DIP Switches
    PRESET_1_OUTPUT_1 = 8,
    PRESET_1_OUTPUT_2 = 9,
    PRESET_1_OUTPUT_3 = 10,
    PRESET_1_OUTPUT_4 = 11,

    PRESET_2_OUTPUT_1 = 12,
    PRESET_2_OUTPUT_2 = 13,
    PRESET_2_OUTPUT_3 = 14,
    PRESET_2_OUTPUT_4 = 15,

    PRESET_3_OUTPUT_1 = 16,
    PRESET_3_OUTPUT_2 = 17,
    PRESET_3_OUTPUT_3 = 18,
    PRESET_3_OUTPUT_4 = 19,

    PRESET_4_OUTPUT_1 = 20,
    PRESET_4_OUTPUT_2 = 21,
    PRESET_4_OUTPUT_3 = 22,
    PRESET_4_OUTPUT_4 = 23,

    // Inverters
    INVERTER_1 = 24,
    INVERTER_2 = 25,
    INVERTER_3 = 26,
    INVERTER_4 = 27,

    PRESET_LABEL_1 = 28,
    PRESET_LABEL_2 = 29,
    PRESET_LABEL_3 = 30,
    PRESET_LABEL_4 = 31,
} PortIndex;


static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
    Tetr4Switch* self = (Tetr4Switch*) malloc(sizeof(Tetr4Switch));

    Tetr4Switch_instantiate(self);

    const char* missing = LV2_HMI_instantiate(self, features);

    if (missing) {
        free(self);
        return NULL;
    }

    Atom_instantiate(self);

    return (LV2_Handle) self;
}

static void connect_port(LV2_Handle instance, uint32_t port, void* data) {
    Tetr4Switch* self = (Tetr4Switch*) instance;

    switch ((PortIndex) port) {
        case OUTPUT_CV_1:
            self->output_cvs[0] = (float*) data; break;
        case OUTPUT_CV_2:
            self->output_cvs[1] = (float*) data; break;
        case OUTPUT_CV_3:
            self->output_cvs[2] = (float*) data; break;
        case OUTPUT_CV_4:
            self->output_cvs[3] = (float*) data; break;
        
        case PRESET_SELECTOR_1:
            self->preset_selectors[0] = (bool*) data; break;
        case PRESET_SELECTOR_2:
            self->preset_selectors[1] = (bool*) data; break;
        case PRESET_SELECTOR_3:
            self->preset_selectors[2] = (bool*) data; break;
        case PRESET_SELECTOR_4:
            self->preset_selectors[3] = (bool*) data; break;

        case PRESET_1_OUTPUT_1:
            self->preset_outputs[0][0] = (bool*) data; break;
        case PRESET_1_OUTPUT_2:
            self->preset_outputs[0][1] = (bool*) data; break;
        case PRESET_1_OUTPUT_3:
            self->preset_outputs[0][2] = (bool*) data; break;
        case PRESET_1_OUTPUT_4:
            self->preset_outputs[0][3] = (bool*) data; break;

        case PRESET_2_OUTPUT_1:
            self->preset_outputs[1][0] = (bool*) data; break;
        case PRESET_2_OUTPUT_2:
            self->preset_outputs[1][1] = (bool*) data; break;
        case PRESET_2_OUTPUT_3:
            self->preset_outputs[1][2] = (bool*) data; break;
        case PRESET_2_OUTPUT_4:
            self->preset_outputs[1][3] = (bool*) data; break;

        case PRESET_3_OUTPUT_1:
            self->preset_outputs[2][0] = (bool*) data; break;
        case PRESET_3_OUTPUT_2:
            self->preset_outputs[2][1] = (bool*) data; break;
        case PRESET_3_OUTPUT_3:
            self->preset_outputs[2][2] = (bool*) data; break;
        case PRESET_3_OUTPUT_4:
            self->preset_outputs[2][3] = (bool*) data; break;

        case PRESET_4_OUTPUT_1:
            self->preset_outputs[3][0] = (bool*) data; break;
        case PRESET_4_OUTPUT_2:
            self->preset_outputs[3][1] = (bool*) data; break;
        case PRESET_4_OUTPUT_3:
            self->preset_outputs[3][2] = (bool*) data; break;
        case PRESET_4_OUTPUT_4:
            self->preset_outputs[3][3] = (bool*) data; break;

        case INVERTER_1:
            self->inverters[0] = (bool*) data; break;
        case INVERTER_2:
            self->inverters[1] = (bool*) data; break;
        case INVERTER_3:
            self->inverters[2] = (bool*) data; break;
        case INVERTER_4:
            self->inverters[3] = (bool*) data; break;

        case PRESET_LABEL_1:
            self->preset_labels[0] = (char*) data; break;
        case PRESET_LABEL_2:
            self->preset_labels[1] = (char*) data; break;
        case PRESET_LABEL_3:
            self->preset_labels[2] = (char*) data; break;
        case PRESET_LABEL_4:
            self->preset_labels[3] = (char*) data; break;
    }
}

static void activate(LV2_Handle instance) {}


static void run(LV2_Handle instance, uint32_t n_samples) {
    Tetr4Switch* self = (Tetr4Switch*) instance;

    LV2_HMI_run(self);
    Atom_run(self);
    
    // Calculate output values
    unsigned int output_coded = self->get_output_signal(self);
    float output_cv_values[TOTAL_OUTPUTS];

    for (unsigned int n = 0; n < TOTAL_OUTPUTS; n++) {
        output_cv_values[n] = output_coded >> n * MAX_TENSION;
    }

    // Update values
    for (uint32_t i = 0; i < n_samples; i++) {
        for (unsigned int id_output = 0; id_output < TOTAL_OUTPUTS; id_output++) {
            self->output_cvs[id_output][i] = output_cv_values[id_output];
        }
    }
}

static void deactivate(LV2_Handle instance) {}

static void cleanup(LV2_Handle instance) {
    free(instance);
}

static const void* extension_data(const char* uri) {
    if (LV2_HMI_is_extension_data_appliable(uri))
        return LV2_HMI_extension_data();
    
    return NULL;
}

static const LV2_Descriptor descriptor = {
    PLUGIN_URI,
    instantiate,
    connect_port,
    activate,
    run,
    deactivate,
    cleanup,
    extension_data
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index) {
    return index == 0
         ? &descriptor
         : NULL;
}
