#ifndef IR_SONY_RM_ED022
#define IR_SONY_RM_ED022

#include <Arduino.h>

const uint16_t SONY_RM_ED022_POWER[77] PROGMEM = {2420, 560, 1224, 564, 626, 566, 1228, 560, 630, 562, 1224, 566, 624, 568, 632, 560, 1226, 562, 626, 566, 624, 568, 632, 560, 630, 25678, 2418, 564, 1222, 568, 632, 558, 1226, 562, 628, 564, 1220, 568, 632, 560, 630, 562, 1224, 566, 624, 568, 632, 560, 630, 562, 628, 25664, 2420, 562, 1222, 566, 624, 568, 1228, 560, 628, 564, 1222, 566, 624, 568, 632, 560, 1224, 564, 626, 566, 624, 568, 632, 560, 630}; // SONY A90
const uint16_t SONY_RM_ED022_INPUT[77] PROGMEM = {2422, 558, 1226, 562, 628, 564, 1222, 568, 632, 560, 630, 562, 1224, 564, 624, 568, 1228, 560, 628, 562, 628, 564, 626, 566, 634, 25658, 2416, 566, 1228, 560, 630, 562, 1222, 566, 626, 566, 632, 560, 1226, 564, 626, 566, 1230, 558, 630, 562, 628, 564, 626, 566, 634, 25674, 2420, 562, 1224, 566, 624, 568, 1228, 560, 630, 562, 626, 566, 1220, 568, 632, 560, 1224, 564, 626, 566, 624, 568, 632, 560, 630}; // SONY A50

const uint16_t SONY_RM_ED022_THEATRE[95] PROGMEM = {2414, 566, 634, 560, 630, 560, 628, 564, 626, 566, 624, 568, 1228, 562, 1224, 564, 1220, 568, 1226, 562, 1224, 564, 626, 566, 1228, 560, 1226, 564, 1222, 566, 634, 19710, 2420, 560, 630, 562, 626, 564, 626, 566, 634, 560, 630, 560, 1224, 564, 1220, 568, 1228, 562, 1224, 564, 1222, 566, 632, 560, 1226, 562, 1222, 566, 1230, 560, 630, 19696, 2414, 568, 632, 560, 630, 562, 628, 564, 626, 566, 634, 558, 1226, 562, 1224, 564, 1220, 568, 1228, 562, 1224, 564, 626, 566, 1228, 560, 1224, 564, 1222, 568, 632};                                                                                                                                                                      // SONY 3EE
const uint16_t SONY_RM_ED022_SKIP_BACK[95] PROGMEM = {2420, 562, 1224, 564, 1220, 568, 634, 558, 1226, 562, 1222, 566, 624, 568, 632, 560, 1226, 564, 1222, 566, 1230, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 1226, 19118, 2418, 562, 1222, 566, 1230, 558, 632, 560, 1224, 564, 1220, 568, 632, 560, 630, 562, 1222, 566, 1230, 558, 1226, 562, 628, 564, 1222, 568, 632, 560, 630, 562, 1222, 19104, 2420, 560, 1226, 564, 1220, 568, 632, 560, 1226, 564, 1222, 566, 634, 558, 632, 560, 1224, 564, 1220, 568, 1228, 560, 630, 562, 1224, 566, 624, 568, 632, 560, 1224};                                                                                                                                                                 // SONY 6CE9
const uint16_t SONY_RM_ED022_PLAY[95] PROGMEM = {2414, 568, 632, 560, 1226, 564, 626, 566, 1230, 558, 1226, 562, 628, 564, 624, 568, 1228, 560, 1226, 562, 1222, 568, 632, 558, 1226, 562, 628, 564, 626, 566, 1230, 19710, 2422, 558, 630, 562, 1224, 566, 624, 568, 1228, 560, 1224, 564, 626, 566, 634, 558, 1228, 562, 1224, 564, 1220, 568, 632, 560, 1224, 564, 626, 566, 624, 568, 1228, 19696, 2414, 566, 634, 558, 1226, 562, 628, 564, 1222, 568, 1228, 562, 628, 562, 628, 564, 1220, 570, 1226, 560, 1224, 564, 626, 566, 1228, 560, 630, 562, 628, 564, 1222};                                                                                                                                                                         // SONY 2CE9
const uint16_t SONY_RM_ED022_SKIP_FORWARD[95] PROGMEM = {2422, 560, 630, 562, 628, 564, 1222, 566, 1228, 560, 1226, 562, 626, 566, 624, 568, 1228, 560, 1224, 564, 1220, 568, 632, 560, 1226, 562, 626, 566, 624, 568, 1228, 19710, 2420, 562, 628, 564, 626, 564, 1230, 558, 1226, 562, 1224, 566, 624, 568, 632, 560, 1226, 562, 1222, 566, 1230, 558, 630, 562, 1224, 566, 624, 568, 632, 560, 1226, 19698, 2414, 568, 632, 560, 632, 562, 1222, 566, 1230, 558, 1228, 560, 628, 564, 626, 566, 1230, 558, 1226, 562, 1222, 566, 624, 568, 1228, 562, 628, 564, 626, 566, 1230};                                                                                                                                                                 // SONY 1CE9
const uint16_t SONY_RM_ED022_SYNC_MENU[95] PROGMEM = {2416, 566, 634, 560, 630, 560, 630, 562, 1222, 566, 1230, 560, 630, 562, 1224, 564, 626, 566, 1228, 560, 630, 562, 1222, 566, 1230, 560, 630, 562, 628, 564, 626, 20910, 2422, 558, 632, 560, 628, 564, 626, 566, 1230, 558, 1226, 562, 628, 564, 1220, 568, 632, 560, 1226, 564, 626, 566, 1230, 558, 1226, 562, 628, 564, 624, 568, 632, 20886, 2414, 568, 632, 560, 630, 562, 628, 564, 1220, 568, 1228, 560, 630, 562, 1222, 566, 624, 568, 1228, 562, 628, 564, 1222, 566, 1228, 560, 630, 562, 626, 564, 626};                                                                                                                                                                          // SONY D58
const uint16_t SONY_RM_ED022_PREVIOUS_TRACK[95] PROGMEM = {2414, 568, 632, 560, 630, 562, 1222, 566, 1230, 558, 1226, 562, 1224, 564, 624, 568, 1228, 560, 1224, 564, 1220, 568, 632, 560, 1226, 562, 628, 564, 626, 566, 1230, 19114, 2422, 560, 630, 562, 626, 566, 1220, 568, 1228, 562, 1224, 566, 1220, 568, 634, 558, 1226, 562, 1222, 566, 1230, 560, 630, 562, 1224, 564, 626, 566, 634, 558, 1226, 19100, 2414, 568, 632, 560, 630, 562, 1224, 564, 1220, 568, 1226, 562, 1224, 564, 626, 566, 1228, 560, 1224, 564, 1222, 566, 634, 558, 1226, 562, 628, 564, 626, 566, 1230};                                                                                                                                                            // SONY 1EE9
const uint16_t SONY_RM_ED022_STOP[127] PROGMEM = {2420, 562, 628, 564, 626, 566, 634, 560, 1226, 562, 1222, 566, 624, 568, 634, 558, 1226, 564, 1222, 566, 1230, 560, 630, 562, 1224, 566, 624, 568, 632, 560, 1226, 20310, 2416, 566, 624, 568, 632, 560, 630, 562, 1224, 566, 1230, 558, 632, 560, 630, 562, 1222, 566, 1230, 558, 1226, 562, 628, 564, 1222, 568, 632, 560, 630, 562, 1224, 20296, 2420, 562, 628, 564, 626, 566, 634, 558, 1226, 562, 1222, 566, 626, 566, 634, 558, 1226, 564, 1222, 566, 1230, 560, 630, 560, 1224, 564, 624, 568, 634, 558, 1226, 20302, 2414, 566, 634, 558, 632, 560, 628, 564, 1222, 568, 1228, 560, 630, 562, 628, 564, 1220, 568, 1228, 560, 1224, 564, 626, 566, 1228, 560, 630, 562, 628, 564, 1222}; // SONY CE9
const uint16_t SONY_RM_ED022_NEXT_TRACK[95] PROGMEM = {2414, 566, 1228, 560, 628, 564, 1222, 566, 1230, 558, 1226, 562, 1222, 566, 624, 568, 1228, 562, 1224, 564, 1220, 568, 632, 560, 1224, 564, 626, 566, 624, 568, 1228, 18520, 2422, 560, 1224, 564, 626, 566, 1228, 560, 1226, 562, 1222, 566, 1230, 558, 630, 562, 1224, 566, 1220, 568, 1228, 562, 628, 564, 1222, 566, 634, 558, 632, 560, 1224, 18508, 2422, 560, 1226, 562, 628, 564, 1220, 568, 1228, 560, 1224, 564, 1222, 568, 632, 560, 1226, 562, 1222, 566, 1230, 558, 630, 562, 1224, 564, 626, 566, 634, 558, 1226};                                                                                                                                                             // SONY 5EE9

const uint16_t SONY_RM_ED022_ARROW_UP[77] PROGMEM = {2420, 560, 628, 564, 626, 566, 1230, 560, 630, 560, 1224, 566, 1220, 568, 1228, 562, 1224, 564, 626, 568, 632, 558, 632, 560, 628, 25068, 2422, 560, 630, 562, 628, 564, 1222, 566, 634, 560, 1226, 562, 1222, 566, 1230, 558, 1226, 562, 628, 564, 626, 566, 634, 558, 632, 25080, 2418, 562, 628, 564, 626, 566, 1228, 560, 630, 562, 1224, 566, 1230, 558, 1228, 562, 1224, 564, 624, 568, 632, 560, 630, 562, 628};      // SONY 2F0
const uint16_t SONY_RM_ED022_ARROW_DOWN[77] PROGMEM = {2414, 568, 1228, 560, 630, 562, 1222, 566, 634, 558, 1228, 562, 1224, 564, 1230, 558, 1226, 562, 628, 564, 626, 566, 634, 558, 632, 24468, 2416, 566, 1230, 558, 630, 562, 1224, 566, 624, 566, 1230, 560, 1226, 562, 1222, 566, 1228, 560, 630, 562, 628, 564, 626, 566, 634, 24482, 2422, 560, 1226, 562, 626, 566, 1230, 558, 632, 560, 1224, 564, 1220, 568, 1228, 560, 1224, 564, 626, 566, 634, 560, 630, 560, 630}; // SONY AF0
const uint16_t SONY_RM_ED022_ARROW_LEFT[77] PROGMEM = {2420, 560, 630, 562, 626, 566, 1230, 558, 630, 562, 1224, 564, 1220, 568, 632, 560, 1224, 564, 626, 566, 624, 568, 632, 560, 630, 25662, 2422, 560, 630, 562, 628, 564, 1222, 568, 632, 560, 1226, 562, 1222, 566, 634, 558, 1226, 562, 628, 564, 626, 566, 624, 568, 632, 25676, 2418, 562, 628, 564, 624, 568, 1228, 560, 628, 564, 1222, 566, 1230, 560, 630, 562, 1224, 564, 626, 566, 634, 558, 630, 562, 628};       // SONY 2D0
const uint16_t SONY_RM_ED022_ARROW_RIGHT[77] PROGMEM = {2418, 564, 1222, 568, 1228, 560, 630, 562, 628, 564, 1220, 568, 1226, 562, 628, 564, 1222, 566, 634, 558, 632, 560, 630, 562, 626, 25070, 2418, 562, 1222, 566, 1230, 558, 632, 560, 628, 564, 1222, 568, 1228, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 630, 562, 628, 25084, 2414, 568, 1228, 562, 1224, 564, 626, 566, 634, 558, 1226, 562, 1222, 566, 624, 568, 1228, 562, 628, 562, 626, 566, 624, 568, 632};   // SONY CD0
const uint16_t SONY_RM_ED022_ENTER[77] PROGMEM = {2422, 560, 1226, 564, 626, 566, 1230, 558, 632, 560, 628, 564, 1222, 566, 1230, 560, 1226, 564, 626, 564, 624, 568, 632, 560, 630, 25066, 2414, 568, 1228, 560, 630, 562, 1222, 566, 634, 558, 632, 560, 1224, 564, 1222, 568, 1228, 560, 630, 562, 628, 564, 626, 566, 634, 25078, 2420, 560, 1224, 564, 628, 564, 1230, 560, 630, 562, 628, 564, 1222, 566, 1228, 560, 1226, 562, 626, 566, 624, 568, 632, 560, 630};         // SONY A70

const uint16_t SONY_RM_ED022_I_MANUAL[95] PROGMEM = {2424, 558, 1226, 562, 1224, 566, 624, 568, 1228, 560, 1224, 564, 1220, 568, 1228, 560, 628, 564, 1222, 566, 634, 558, 1226, 562, 1222, 566, 624, 568, 632, 560, 630, 19116, 2420, 562, 1222, 566, 1230, 558, 630, 562, 1224, 564, 1220, 568, 1226, 562, 1224, 566, 624, 568, 1228, 560, 630, 562, 1222, 566, 1228, 560, 630, 562, 628, 564, 626, 19106, 2420, 562, 1222, 566, 1230, 560, 630, 562, 1224, 566, 1220, 568, 1228, 562, 1224, 564, 624, 568, 1228, 560, 628, 564, 1222, 566, 1230, 558, 630, 562, 628, 564, 626};                                  // SONY 6F58
const uint16_t SONY_RM_ED022_SCENE[95] PROGMEM = {2422, 560, 630, 562, 628, 564, 626, 566, 1230, 558, 1226, 562, 1222, 566, 1230, 560, 630, 562, 1222, 566, 624, 568, 1228, 562, 1224, 564, 626, 566, 634, 558, 632, 20308, 2416, 564, 626, 566, 634, 558, 630, 562, 1224, 566, 1220, 568, 1228, 560, 1224, 564, 626, 566, 1228, 560, 630, 562, 1224, 566, 1230, 558, 632, 560, 630, 562, 626, 20298, 2418, 562, 628, 564, 626, 566, 634, 560, 1226, 562, 1222, 566, 1230, 558, 1226, 562, 628, 564, 1220, 568, 632, 560, 1226, 562, 1222, 566, 634, 558, 630, 562, 628};                                           // SONY F58
const uint16_t SONY_RM_ED022_ASPECT_RATIO[95] PROGMEM = {2422, 560, 1226, 538, 656, 562, 1220, 568, 1228, 560, 1224, 564, 1220, 568, 634, 558, 630, 562, 628, 564, 1220, 568, 632, 560, 630, 562, 1224, 566, 624, 568, 1228, 19712, 2420, 562, 1222, 566, 624, 568, 1228, 562, 1224, 564, 1220, 568, 1226, 562, 628, 564, 626, 566, 624, 568, 1226, 562, 628, 564, 626, 566, 1230, 558, 632, 560, 1224, 19700, 2420, 560, 1224, 564, 626, 566, 1228, 560, 1226, 564, 1222, 566, 1228, 560, 630, 562, 626, 566, 624, 568, 1228, 560, 630, 562, 628, 564, 1220, 568, 632, 560, 1226};                                 // SONY 5E25
const uint16_t SONY_RM_ED022_DIGITAL_ANALOG[95] PROGMEM = {2418, 564, 1222, 566, 634, 558, 1226, 562, 1222, 566, 624, 568, 632, 560, 630, 562, 1222, 566, 1230, 558, 1226, 562, 628, 564, 1220, 568, 1228, 562, 1224, 564, 626, 19120, 2414, 568, 1228, 560, 630, 562, 1222, 566, 1228, 560, 630, 562, 628, 564, 626, 566, 1230, 560, 1224, 562, 1222, 566, 634, 558, 1226, 562, 1224, 566, 1230, 558, 630, 19102, 2412, 568, 1228, 560, 628, 564, 1222, 566, 1230, 560, 630, 562, 626, 564, 626, 566, 1228, 560, 1226, 564, 1222, 566, 624, 568, 1228, 562, 1224, 564, 1220, 568, 632};                            // SONY 58EE
const uint16_t SONY_RM_ED022_FAVOURITES[95] PROGMEM = {2418, 564, 626, 566, 1230, 558, 1226, 562, 628, 564, 1222, 566, 1228, 562, 1224, 564, 1220, 568, 1228, 560, 1224, 564, 624, 568, 1228, 560, 1226, 564, 1222, 568, 632, 17904, 2420, 562, 628, 564, 1220, 568, 1228, 560, 628, 564, 1222, 568, 1228, 560, 1226, 564, 1222, 566, 1228, 560, 1224, 564, 626, 566, 1230, 558, 1226, 562, 1224, 566, 624, 17930, 2414, 568, 632, 560, 1226, 562, 1222, 566, 624, 568, 1226, 562, 1224, 566, 1220, 568, 1228, 562, 1224, 564, 1232, 558, 632, 560, 1224, 564, 1222, 568, 1228, 560, 630};                          // SONY 37EE
const uint16_t SONY_RM_ED022_GUIDE[95] PROGMEM = {2424, 560, 1226, 562, 1224, 566, 624, 568, 1228, 560, 1224, 564, 626, 566, 1228, 560, 630, 562, 628, 564, 1222, 568, 632, 560, 630, 562, 1222, 566, 624, 568, 1228, 19712, 2418, 562, 1222, 566, 1230, 558, 630, 562, 1224, 564, 1220, 568, 632, 560, 1226, 562, 626, 566, 624, 568, 1228, 562, 628, 564, 626, 566, 1230, 558, 632, 560, 1224, 19700, 2420, 560, 1224, 564, 1220, 568, 632, 560, 1226, 564, 1222, 566, 634, 558, 1226, 562, 628, 564, 626, 566, 1228, 560, 630, 562, 628, 564, 1220, 568, 632, 560, 1226};                                        // SONY 6D25
const uint16_t SONY_RM_ED022_INFO[103] PROGMEM = {2420, 562, 628, 564, 1222, 566, 634, 558, 1226, 562, 1224, 564, 1220, 568, 632, 560, 1224, 564, 626, 566, 624, 568, 632, 560, 630, 25082, 2416, 564, 624, 568, 1228, 560, 630, 562, 1224, 566, 1230, 558, 1226, 562, 628, 564, 1220, 568, 632, 560, 630, 562, 628, 564, 626, 25070, 2418, 564, 626, 566, 1230, 558, 630, 562, 1224, 564, 1222, 568, 1228, 560, 628, 564, 1220, 568, 634, 558, 630, 562, 628, 564, 626, 25078, 2422, 560, 628, 562, 1222, 566, 634, 558, 1226, 562, 1224, 566, 1230, 558, 632, 560, 1224, 564, 626, 566, 634, 558, 630, 562, 628}; // SONY 5D0
const uint16_t SONY_RM_ED022_RETURN[95] PROGMEM = {2416, 564, 1220, 568, 1230, 560, 628, 564, 626, 566, 624, 568, 1228, 562, 628, 564, 1222, 568, 1228, 560, 1226, 564, 626, 566, 1230, 558, 630, 562, 628, 564, 1222, 19718, 2422, 560, 1226, 564, 1222, 566, 634, 558, 630, 562, 628, 564, 1222, 568, 632, 560, 1226, 562, 1222, 568, 1228, 560, 630, 562, 1224, 566, 624, 568, 632, 560, 1226, 19698, 2422, 560, 1224, 564, 1222, 568, 632, 560, 630, 562, 628, 564, 1220, 568, 632, 560, 1226, 564, 1222, 566, 1230, 560, 630, 560, 1224, 566, 624, 568, 632, 560, 1226};                                       // SONY 62E9
const uint16_t SONY_RM_ED022_OPTIONS[95] PROGMEM = {2416, 566, 624, 568, 1228, 560, 1224, 564, 626, 566, 1228, 560, 1224, 564, 626, 566, 1230, 560, 1226, 562, 1222, 566, 624, 568, 1228, 562, 628, 564, 626, 566, 1230, 19114, 2420, 560, 630, 562, 1224, 566, 1230, 558, 632, 560, 1224, 564, 1220, 568, 632, 560, 1224, 564, 1222, 566, 1230, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 1226, 19102, 2424, 558, 630, 562, 1224, 564, 1220, 568, 632, 560, 1226, 562, 1222, 566, 634, 558, 1226, 562, 1224, 566, 1230, 558, 632, 560, 1224, 564, 626, 566, 634, 558, 1226};                                   // SONY 36E9
const uint16_t SONY_RM_ED022_HOME[77] PROGMEM = {2420, 562, 628, 564, 626, 566, 624, 568, 632, 560, 630, 562, 1222, 566, 1230, 560, 1226, 562, 628, 564, 624, 568, 634, 558, 630, 26258, 2420, 562, 628, 564, 626, 566, 634, 558, 632, 560, 630, 562, 1222, 566, 1228, 560, 1226, 562, 628, 564, 624, 568, 634, 558, 630, 26274, 2414, 566, 634, 560, 630, 560, 628, 564, 626, 566, 624, 568, 1228, 562, 1224, 564, 1220, 568, 632, 560, 630, 562, 626, 566, 624};                                                                                                                                                  // SONY 70

const uint16_t SONY_RM_ED022_RED[95] PROGMEM = {2414, 568, 1228, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 1224, 564, 626, 566, 1230, 560, 1226, 562, 1222, 566, 634, 558, 1226, 562, 628, 564, 626, 566, 1228, 19712, 2420, 560, 1224, 566, 624, 568, 1228, 560, 630, 562, 628, 564, 1220, 568, 632, 558, 1226, 562, 1222, 566, 1230, 560, 630, 562, 1224, 564, 626, 566, 634, 558, 1226, 19698, 2422, 558, 1226, 562, 628, 564, 1220, 568, 632, 560, 630, 562, 1222, 566, 624, 568, 1228, 560, 1224, 564, 1220, 568, 632, 560, 1226, 564, 626, 566, 624, 568, 1228};                                                                                                                                                                          // SONY 52E9
const uint16_t SONY_RM_ED022_GREEN[95] PROGMEM = {2422, 560, 630, 562, 1224, 564, 1220, 568, 632, 560, 628, 564, 1222, 566, 624, 568, 1228, 562, 1224, 566, 1220, 568, 632, 560, 1224, 564, 626, 566, 624, 568, 1228, 19712, 2418, 564, 626, 566, 1230, 558, 1228, 562, 628, 564, 626, 566, 1230, 558, 632, 560, 1224, 566, 1220, 570, 1226, 562, 628, 564, 1222, 566, 634, 558, 630, 486, 1300, 19700, 2422, 560, 628, 562, 1222, 566, 1230, 558, 632, 560, 628, 564, 1222, 568, 632, 560, 1226, 562, 1224, 566, 1230, 558, 632, 560, 1224, 564, 626, 566, 624, 568, 1226};                                                                                                                                                                        // SONY 32E9
const uint16_t SONY_RM_ED022_YELLOW[95] PROGMEM = {2416, 566, 1230, 558, 1226, 562, 1224, 566, 624, 568, 632, 560, 1226, 562, 626, 566, 1230, 560, 1226, 562, 1222, 566, 624, 568, 1228, 560, 628, 564, 626, 566, 1230, 19112, 2422, 560, 1226, 562, 1222, 566, 1230, 558, 630, 562, 628, 564, 1222, 566, 634, 560, 1226, 562, 1224, 564, 1230, 558, 630, 562, 1224, 566, 624, 566, 634, 558, 1226, 19100, 2414, 568, 1228, 560, 1224, 566, 1220, 568, 634, 558, 630, 562, 1224, 566, 624, 568, 1228, 560, 1226, 564, 1222, 566, 634, 558, 1226, 562, 628, 564, 624, 568, 1228};                                                                                                                                                                    // SONY 72E9
const uint16_t SONY_RM_ED022_BLUE[127] PROGMEM = {2420, 562, 626, 566, 624, 568, 1228, 562, 628, 562, 628, 564, 1220, 568, 632, 560, 1226, 564, 1222, 566, 1228, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 1226, 20310, 2416, 566, 624, 568, 632, 560, 1224, 564, 626, 566, 622, 568, 1228, 562, 628, 564, 1220, 568, 1228, 560, 1224, 564, 626, 566, 1230, 558, 632, 560, 630, 566, 1218, 20298, 2416, 564, 626, 566, 634, 558, 1226, 562, 628, 564, 626, 566, 1230, 560, 630, 562, 1224, 564, 1220, 568, 1226, 562, 628, 564, 1222, 568, 632, 560, 630, 562, 1222, 20304, 2422, 560, 630, 562, 628, 566, 1220, 568, 632, 560, 630, 562, 1222, 566, 624, 568, 1228, 460, 1324, 564, 1220, 568, 632, 560, 1224, 564, 626, 566, 624, 568, 1228}; // SONY 12E9

const uint16_t SONY_RM_ED022_NUMERIC_1[77] PROGMEM = {2414, 568, 632, 560, 630, 562, 628, 564, 626, 566, 634, 558, 632, 560, 628, 564, 1222, 566, 624, 568, 632, 560, 630, 562, 628, 27470, 2422, 560, 630, 562, 628, 564, 624, 568, 634, 558, 630, 562, 628, 564, 626, 566, 1230, 558, 632, 562, 628, 562, 626, 566, 624, 27456, 2414, 568, 632, 560, 630, 562, 628, 564, 624, 568, 632, 560, 630, 562, 628, 564, 1222, 566, 634, 558, 630, 562, 628, 564, 626};          // SONY 10
const uint16_t SONY_RM_ED022_NUMERIC_2[77] PROGMEM = {2418, 564, 1220, 568, 632, 560, 630, 562, 628, 564, 626, 566, 632, 560, 630, 562, 1224, 564, 626, 566, 634, 558, 630, 562, 628, 26872, 2412, 568, 1228, 562, 628, 564, 626, 566, 624, 568, 632, 560, 630, 562, 628, 564, 1222, 566, 632, 560, 630, 562, 628, 564, 626, 26860, 2416, 566, 1230, 558, 632, 560, 628, 564, 626, 566, 624, 568, 632, 560, 630, 562, 1224, 564, 626, 568, 632, 558, 630, 562, 628};       // SONY 810
const uint16_t SONY_RM_ED022_NUMERIC_3[77] PROGMEM = {2420, 562, 628, 564, 1220, 568, 632, 560, 630, 562, 628, 564, 626, 566, 634, 558, 1226, 562, 630, 562, 626, 566, 634, 558, 632, 26870, 2418, 564, 626, 566, 1230, 560, 628, 562, 628, 564, 626, 566, 634, 558, 632, 560, 1224, 566, 624, 566, 634, 558, 632, 562, 628, 26856, 2418, 562, 626, 566, 1230, 558, 632, 560, 628, 564, 626, 566, 624, 568, 632, 560, 1226, 562, 626, 566, 624, 568, 632, 560, 630};       // SONY 410
const uint16_t SONY_RM_ED022_NUMERIC_4[77] PROGMEM = {2414, 568, 1228, 560, 1224, 564, 626, 566, 634, 558, 632, 560, 628, 564, 626, 566, 1230, 558, 632, 560, 628, 564, 626, 566, 624, 26280, 2420, 562, 1224, 564, 1230, 560, 630, 562, 628, 562, 628, 564, 624, 568, 632, 560, 1224, 564, 626, 566, 624, 568, 632, 560, 630, 26258, 2422, 560, 1226, 564, 1222, 566, 634, 558, 632, 562, 628, 562, 626, 566, 624, 568, 1228, 560, 628, 562, 628, 564, 624, 568, 634};    // SONY C10
const uint16_t SONY_RM_ED022_NUMERIC_5[77] PROGMEM = {2416, 564, 626, 568, 632, 558, 1226, 562, 628, 564, 626, 566, 634, 558, 630, 562, 1224, 564, 626, 566, 634, 558, 632, 560, 630, 26856, 2418, 562, 628, 564, 626, 566, 1228, 560, 630, 562, 628, 564, 626, 566, 634, 558, 1226, 562, 628, 564, 626, 566, 634, 558, 632, 26868, 2416, 566, 624, 568, 632, 560, 1224, 564, 626, 566, 624, 568, 632, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 630, 562, 628};       // SONY 210
const uint16_t SONY_RM_ED022_NUMERIC_6[77] PROGMEM = {2414, 566, 1230, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 630, 562, 628, 564, 1222, 566, 634, 558, 632, 560, 628, 564, 626, 26262, 2418, 564, 1222, 566, 634, 558, 1226, 562, 628, 564, 626, 566, 634, 558, 632, 560, 1224, 564, 626, 566, 634, 558, 632, 560, 628, 26276, 2414, 566, 1228, 560, 630, 562, 1224, 564, 624, 568, 632, 560, 630, 562, 628, 564, 1220, 568, 632, 560, 630, 562, 628, 564, 624};    // SONY A10
const uint16_t SONY_RM_ED022_NUMERIC_7[77] PROGMEM = {2422, 560, 630, 562, 1224, 566, 1230, 558, 632, 560, 630, 562, 626, 566, 624, 568, 1228, 562, 628, 564, 626, 566, 624, 568, 632, 26256, 2422, 560, 630, 562, 1224, 564, 1220, 568, 632, 560, 630, 562, 626, 566, 624, 568, 1228, 560, 630, 562, 626, 566, 624, 568, 632, 26272, 2416, 564, 626, 566, 1228, 560, 1224, 564, 626, 566, 624, 568, 632, 560, 628, 562, 1224, 566, 624, 568, 632, 560, 628, 564, 626};    // SONY 610
const uint16_t SONY_RM_ED022_NUMERIC_8[77] PROGMEM = {2414, 568, 1226, 562, 1224, 566, 1220, 568, 632, 560, 630, 562, 626, 566, 624, 568, 1226, 560, 630, 562, 628, 564, 624, 568, 632, 25660, 2414, 568, 1228, 560, 1226, 564, 1220, 568, 632, 560, 630, 562, 628, 564, 626, 568, 1228, 560, 630, 562, 626, 566, 624, 568, 632, 25674, 2418, 564, 1222, 566, 1228, 560, 1226, 564, 626, 566, 624, 568, 632, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 630, 564, 626}; // SONY E10
const uint16_t SONY_RM_ED022_NUMERIC_9[77] PROGMEM = {2416, 566, 624, 568, 632, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 630, 562, 628, 26872, 2424, 558, 630, 562, 628, 564, 626, 566, 1230, 558, 632, 560, 630, 564, 626, 566, 1220, 568, 632, 560, 630, 562, 628, 564, 626, 26860, 2414, 566, 634, 558, 632, 560, 628, 564, 1222, 566, 634, 560, 630, 560, 628, 564, 1222, 566, 634, 558, 630, 562, 628, 564, 626};       // SONY 110
const uint16_t SONY_RM_ED022_NUMERIC_0[77] PROGMEM = {2416, 566, 1230, 560, 630, 562, 628, 564, 1220, 568, 632, 560, 630, 562, 628, 564, 1222, 566, 634, 558, 632, 560, 628, 564, 626, 26262, 2418, 564, 1220, 568, 632, 560, 630, 562, 1224, 564, 624, 568, 634, 558, 630, 562, 1222, 566, 624, 568, 632, 560, 630, 562, 628, 26274, 2414, 568, 1228, 562, 628, 562, 628, 564, 1230, 558, 632, 560, 628, 564, 626, 566, 1230, 558, 632, 560, 630, 562, 626, 566, 626};    // SONY 910

const uint16_t SONY_RM_ED022_TELETEXT[77] PROGMEM = {2422, 560, 1224, 564, 1222, 566, 1228, 560, 1224, 564, 1222, 566, 1228, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 630, 562, 628, 23874, 2412, 568, 1228, 560, 1224, 566, 1220, 568, 1228, 562, 1224, 564, 1220, 568, 632, 560, 1226, 562, 626, 566, 624, 568, 632, 560, 630, 23890, 2420, 560, 1224, 564, 1220, 568, 1228, 560, 1224, 564, 1222, 568, 1228, 560, 630, 564, 1222, 566, 624, 568, 632, 560, 630, 562, 628};                                                                                            // SONY FD0
const uint16_t SONY_RM_ED022_TELETEXT2[95] PROGMEM = {2418, 562, 628, 564, 624, 568, 634, 558, 1226, 562, 626, 564, 1220, 568, 632, 560, 1224, 564, 1222, 566, 1230, 560, 630, 562, 1224, 564, 624, 568, 634, 558, 1226, 20294, 2422, 560, 630, 562, 628, 564, 626, 566, 1230, 560, 630, 562, 1224, 566, 624, 568, 1228, 560, 1226, 564, 1222, 566, 634, 558, 1226, 562, 628, 564, 626, 566, 1228, 20306, 2420, 562, 628, 564, 626, 566, 624, 568, 1226, 562, 628, 564, 1220, 566, 634, 558, 1226, 562, 1222, 566, 1230, 558, 630, 562, 1224, 564, 626, 566, 624, 568, 1228}; // SONY AE9

const uint16_t SONY_RM_ED022_MUTE[77] PROGMEM = {2420, 562, 626, 566, 624, 568, 1228, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 1226, 564, 626, 566, 624, 568, 632, 560, 630, 26274, 2416, 564, 624, 568, 632, 560, 1226, 564, 626, 566, 1230, 558, 632, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 630, 562, 628, 26262, 2418, 562, 626, 566, 624, 568, 1228, 560, 630, 562, 1222, 566, 624, 568, 632, 560, 1224, 564, 626, 566, 624, 568, 632, 560, 630}; // SONY 290

const uint16_t SONY_RM_ED022_VOLUME_UP[77] PROGMEM = {2416, 566, 624, 566, 1228, 560, 630, 562, 628, 564, 1220, 568, 632, 560, 630, 562, 1224, 564, 624, 568, 634, 558, 630, 562, 628, 26276, 2416, 566, 634, 558, 1226, 562, 628, 564, 626, 566, 1230, 558, 632, 560, 628, 564, 1222, 566, 632, 560, 632, 560, 628, 564, 626, 26262, 2418, 564, 626, 566, 1230, 558, 632, 560, 630, 562, 1222, 566, 634, 558, 632, 560, 1224, 566, 624, 566, 624, 568, 632, 560, 630};      // SONY 490
const uint16_t SONY_RM_ED022_VOLUME_DOWN[77] PROGMEM = {2418, 562, 1222, 566, 1230, 558, 630, 562, 628, 564, 1222, 566, 634, 558, 632, 560, 1224, 564, 624, 568, 632, 560, 632, 484, 704, 25662, 2422, 560, 1226, 564, 1222, 568, 632, 558, 632, 560, 1224, 564, 626, 566, 634, 558, 1226, 562, 628, 564, 626, 566, 634, 558, 632, 25676, 2420, 562, 1222, 566, 1230, 558, 632, 560, 628, 564, 1220, 568, 632, 560, 630, 562, 1224, 564, 626, 566, 634, 560, 630, 560, 628}; // SONY C90

const uint16_t SONY_RM_ED022_PROGRAM_UP[77] PROGMEM = {2418, 562, 628, 566, 634, 560, 630, 564, 626, 568, 1226, 562, 626, 568, 632, 560, 1224, 566, 624, 570, 630, 564, 626, 568, 632, 26756, 2410, 570, 630, 564, 626, 568, 632, 560, 630, 564, 1220, 570, 630, 564, 626, 568, 1228, 562, 628, 566, 634, 560, 630, 564, 626, 26762, 2414, 566, 624, 570, 630, 564, 626, 568, 632, 562, 1224, 566, 632, 560, 630, 566, 1220, 570, 630, 562, 626, 568, 632, 562, 628};      // SONY 90
const uint16_t SONY_RM_ED022_PROGRAM_DOWN[77] PROGMEM = {2418, 562, 1222, 566, 634, 560, 628, 566, 624, 570, 1226, 564, 626, 568, 632, 560, 1224, 566, 624, 570, 632, 562, 626, 568, 634, 26160, 2412, 568, 1228, 562, 628, 566, 624, 570, 630, 564, 1222, 568, 632, 562, 628, 566, 1230, 560, 630, 564, 626, 568, 632, 562, 628, 26164, 2418, 562, 1222, 568, 632, 562, 628, 564, 624, 570, 1226, 564, 626, 568, 634, 560, 1224, 564, 624, 570, 630, 564, 626, 568, 632}; // SONY 890

const uint16_t SONY_RM_ED022_AUDIO[129] PROGMEM = {2414, 568, 1226, 562, 1224, 564, 1220, 568, 632, 560, 1224, 564, 626, 566, 626, 566, 1228, 562, 628, 564, 626, 566, 624, 568, 632, 25078, 2422, 560, 1226, 562, 1222, 566, 1228, 560, 630, 562, 1224, 564, 626, 566, 634, 558, 1226, 562, 628, 564, 626, 566, 634, 560, 630, 25064, 2414, 566, 1230, 558, 1226, 562, 1222, 566, 624, 568, 1228, 562, 628, 564, 626, 566, 1230, 558, 632, 560, 630, 562, 626, 566, 624, 25078, 2422, 558, 1226, 562, 1222, 566, 1230, 560, 630, 562, 1224, 564, 626, 566, 634, 560, 1226, 562, 628, 564, 626, 566, 624, 568, 632, 25072, 2418, 562, 1222, 566, 1230, 558, 1228, 562, 628, 564, 1220, 568, 634, 558, 632, 560, 1224, 564, 624, 568, 632, 560, 632, 560, 630}; // SONY E90

#endif