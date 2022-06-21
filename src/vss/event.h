//
// Created by caiiiycuk on 17.05.2022.
//

#ifndef VANGERS_SYS_EVENT_H
#define VANGERS_SYS_EVENT_H

namespace vss {
extern const char *SYS_EVENT_READY;
extern const char *SYS_EVENT_SCALED_RENDERER_CHANGED;
extern const char *SYS_EVENT_RUNTIME_OBJECT_CHANGED;

struct Event {
  const char *type;

  union {
    bool bool0;

    // SYS_EVENT_SCALED_RENDERER_CHANGED
    bool scaledRenderer;
  };

  union {
    int int0;

    // SYS_EVENT_RUNTIME_OBJECT_CHANGED
    //
    // RTO_GAME_QUANT_ID         // 1
    // RTO_LOADING1_ID,		     // 2
    // RTO_LOADING2_ID,		     // 3
    // RTO_LOADING3_ID,		     // 4
    // RTO_MAIN_MENU_ID,		 // 5
    // RTO_FIRST_ESCAVE_ID,		 // 6
    // RTO_FIRST_ESCAVE_OUT_ID,	 // 7
    // RTO_ESCAVE_ID,			 // 8
    // RTO_ESCAVE_OUT_ID,		 // 9
    // RTO_PALETTE_TRANSFORM_ID, // 10
    // RTO_SHOW_IMAGE_ID,		 // 11
    // RTO_SHOW_AVI_ID,		     // 12
    int runtimeObjectId;
  };
};
}  // namespace vangers

#endif  // VANGERS_SYS_EVENT_H
