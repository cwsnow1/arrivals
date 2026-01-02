#include "cta.h"

#define ID_OFFSET (30000)
#define INDEX(id) ((id) - ID_OFFSET)

#define RED_LINE_OFFSET     (13)
#define RED_LINE_COUNT      (66)
#define YELLOW_LINE_OFFSET  (RED_LINE_OFFSET + RED_LINE_COUNT)
#define YELLOW_LINE_COUNT   (12)
#define PURPLE_LINE_OFFSET  (YELLOW_LINE_OFFSET + YELLOW_LINE_COUNT)

static const stop_t stops[] = {
    [INDEX(_18TH_54TH_CERMAK_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = _18TH_PINK,
            .location = {41.857908, -87.669147}
        }
    },
    [INDEX(_18TH_LOOP_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = _18TH_PINK,
            .location = {41.857908, -87.669147}
        }
    },
    [INDEX(_35TH_ARCHER_LOOP_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = _35TH_ARCHER_ORANGE,
            .location = {41.829353, -87.680622}
        }
    },
    [INDEX(_35TH_ARCHER_MIDWAY_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = _35TH_ARCHER_ORANGE,
            .location = {41.829353, -87.680622}
        }
    },
    [INDEX(_35_BRONZEVILLE_IIT_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = _35TH_BRONZEVILLE_IIT_GREEN,
            .location = {41.831677, -87.625826}
        }
    },
    [INDEX(_35_BRONZEVILLE_IIT_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = _35TH_BRONZEVILLE_IIT_GREEN,
            .location = {41.831677, -87.625826}
        }
    },
    [INDEX(_43RD_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = _43RD_GREEN,
            .location = {41.816462, -87.619021}
        }
    },
    [INDEX(_43RD_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = _43RD_GREEN,
            .location = {41.816462, -87.619021}
        }
    },
    [INDEX(_47TH_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = _47TH_GREEN,
            .location = {41.809209, -87.618826}
        }
    },
    [INDEX(_47TH_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = _47TH_RED,
            .location = {41.810318, -87.63094}
        },
        .led = {
            .start = RED_LINE_OFFSET + 16,
            .station = RED_LINE_OFFSET + 18,
            .count = 2
        }
    },
    [INDEX(_47TH_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = _47TH_RED,
            .location = {41.810318, -87.63094}
        },
        .led = {
            .start = RED_LINE_OFFSET + 21,
            .station = RED_LINE_OFFSET + 18,
            .count = -3
        }
    },
    [INDEX(_47TH_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = _47TH_GREEN,
            .location = {41.809209, -87.618826}
        }
    },
    [INDEX(_51ST_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = _51ST_GREEN,
            .location = {41.80209, -87.618487}
        }
    },
    [INDEX(_51ST_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = _51ST_GREEN,
            .location = {41.80209, -87.618487}
        }
    },
    [INDEX(_54TH_CERMAK_TERMINAL_ARRIVAL)] = {
        .line = PINK_LINE,
        .station = {
            .id = _54TH_CERMAK_PINK,
            .location = {41.85177331, -87.75669201}
        }
    },
    [INDEX(_54TH_CERMAK_LOOP_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = _54TH_CERMAK_PINK,
            .location = {41.85177331, -87.75669201}
        }
    },
    [INDEX(_63RD_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = _63RD_RED,
            .location = {41.780536, -87.630952}
        },
        .led = {
            .start = RED_LINE_OFFSET + 10,
            .station = RED_LINE_OFFSET + 11,
            .count = 1
        }
    },
    [INDEX(_63RD_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = _63RD_RED,
            .location = {41.780536, -87.630952}
        },
        .led = {
            .start = RED_LINE_OFFSET + 14,
            .station = RED_LINE_OFFSET + 11,
            .count = -3
        }
    },
    [INDEX(_69TH_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = _69TH_RED,
            .location = {41.768367, -87.625724}
        },
        .led = {
            .start = RED_LINE_OFFSET + 7,
            .station = RED_LINE_OFFSET + 9,
            .count = 2
        }
    },
    [INDEX(_69TH_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = _69TH_RED,
            .location = {41.768367, -87.625724}
        },
        .led = {
            .start = RED_LINE_OFFSET + 10,
            .station = RED_LINE_OFFSET + 9,
            .count = -1
        }
    },
    [INDEX(_79TH_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = _79TH_RED,
            .location = {41.750419, -87.625112}
        },
        .led = {
            .start = RED_LINE_OFFSET + 8,
            .station = RED_LINE_OFFSET + 6,
            .count = -2
        }
    },
    [INDEX(_79TH_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = _79TH_RED,
            .location = {41.750419, -87.625112}
        },
        .led = {
            .start = RED_LINE_OFFSET + 4,
            .station = RED_LINE_OFFSET + 6,
            .count = 2
        }
    },
    [INDEX(_87TH_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = _87TH_RED,
            .location = {41.735372, -87.624717}
        },
        .led = {
            .start = RED_LINE_OFFSET + 1,
            .station = RED_LINE_OFFSET + 3,
            .count = 2,
        },
    },
    [INDEX(_87TH_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = _87TH_RED,
            .location = {41.735372, -87.624717}
        },
        .led = {
            .start = RED_LINE_OFFSET + 5,
            .station = RED_LINE_OFFSET + 3,
            .count = -2,
        },
    },
    [INDEX(_95TH_DAN_RYAN_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = _95TH_DAN_RYAN_RED,
            .location = {41.722377, -87.624342}
        },
        .led = {
            .start = RED_LINE_OFFSET,
            .station = RED_LINE_OFFSET,
            .count = 0,
        },
    },
    [INDEX(_95TH_DAN_RYAN_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = _95TH_DAN_RYAN_RED,
            .location = {41.722377, -87.624342}
        },
        .led = {
            .start = RED_LINE_OFFSET + 2,
            .station = RED_LINE_OFFSET,
            .count = -2,
        },
    },
    [INDEX(ADAMS_WABASH_OUTER_LOOP)] = {
        .line = GREEN_LINE | BROWN_LINE,
        .station = {
            .id = ADAMS_WABASH_BROWN_GREEN_ORANGE_PINK_PURPLE,
            .location = {41.879507, -87.626037}
        }
    },
    [INDEX(ADAMS_WABASH_INNER_LOOP)] = {
        .line = GREEN_LINE | PURPLE_LINE | PINK_LINE | ORANGE_LINE,
        .station = {
            .id = ADAMS_WABASH_BROWN_GREEN_ORANGE_PINK_PURPLE,
            .location = {41.879507, -87.626037}
        }
    },
    [INDEX(ADDISON_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = ADDISON_RED,
            .location = {41.947428, -87.653626}
        },
        .led = {
            .start = RED_LINE_OFFSET + 52,
            .station = RED_LINE_OFFSET + 52,
            .count = 0
        }
    },
    [INDEX(ADDISON_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = ADDISON_BROWN,
            .location = {41.947028, -87.674642}
        }
    },
    [INDEX(ADDISON_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = ADDISON_BLUE,
            .location = {41.94738, -87.71906}
        }
    },
    [INDEX(ADDISON_KIMBALL_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = ADDISON_BROWN,
            .location = {41.947028, -87.674642}
        }
    },
    [INDEX(ADDISON_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = ADDISON_BLUE,
            .location = {41.94738, -87.71906}
        }
    },
    [INDEX(ADDISON_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = ADDISON_RED,
            .location = {41.947428, -87.653626}
        },
        .led = {
            .start = RED_LINE_OFFSET + 51,
            .station = RED_LINE_OFFSET + 52,
            .count = 1
        }
    },
    [INDEX(ARGYLE_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = ARGYLE_RED,
            .location = {41.973453, -87.65853}
        },
        .led = {
            .start = RED_LINE_OFFSET + 56,
            .station = RED_LINE_OFFSET + 56,
            .count = 0
        }
    },
    [INDEX(ARGYLE_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = ARGYLE_RED,
            .location = {41.973453, -87.65853}
        },
        .led = {
            .start = RED_LINE_OFFSET + 56,
            .station = RED_LINE_OFFSET + 56,
            .count = 0
        }
    },
    [INDEX(ARMITAGE_KIMBALL_LINDEN_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = ARMITAGE_BROWN_PURPLE,
            .location = {41.918217, -87.652644}
        }
    },
    [INDEX(ARMITAGE_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = ARMITAGE_BROWN_PURPLE,
            .location = {41.918217, -87.652644}
        }
    },
    [INDEX(ASHLAND_HARLEM_54TH_CERMAK_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = ASHLAND_GREEN_PINK,
            .location = {41.885269, -87.666969}
        }
    },
    [INDEX(ASHLAND_LOOP_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = ASHLAND_GREEN_PINK,
            .location = {41.885269, -87.666969}
        }
    },
    [INDEX(ASHLAND_MIDWAY_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = ASHLAND_ORANGE,
            .location = {41.839234, -87.665317}
        }
    },
    [INDEX(ASHLAND_LOOP_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = ASHLAND_ORANGE,
            .location = {41.839234, -87.665317}
        }
    },
    [INDEX(ASHLAND_63RD_TERMINAL_ARRIVAL)] = {
        .line = GREEN_LINE,
        .station = {
            .id = ASHLAND_63RD_GREEN,
            .location = {41.77886, -87.663766}
        }
    },
    [INDEX(ASHLAND_63RD_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = ASHLAND_63RD_GREEN,
            .location = {41.77886, -87.663766}
        }
    },
    [INDEX(AUSTIN_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = AUSTIN_BLUE,
            .location = {41.870851, -87.776812}
        }
    },
    [INDEX(AUSTIN_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = AUSTIN_BLUE,
            .location = {41.870851, -87.776812}
        }
    },
    [INDEX(AUSTIN_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = AUSTIN_GREEN,
            .location = {41.887293, -87.774135}
        }
    },
    [INDEX(AUSTIN_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = AUSTIN_GREEN,
            .location = {41.887293, -87.774135}
        }
    },
    [INDEX(BELMONT_OHARE_BRANCH_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = BELMONT_BLUE,
            .location = {41.938132, -87.712359}
        }
    },
    [INDEX(BELMONT_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = BELMONT_RED_BROWN_PURPLE,
            .location = {41.939751, -87.65338}
        }
    },
    [INDEX(BELMONT_OHARE_BRANCH_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = BELMONT_BLUE,
            .location = {41.938132, -87.712359}
        }
    },
    [INDEX(BELMONT_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = BELMONT_RED_BROWN_PURPLE,
            .location = {41.939751, -87.65338}
        },
        .led = {
            .start = RED_LINE_OFFSET + 51,
            .station = RED_LINE_OFFSET + 50,
            .count = -1
        }
    },
    [INDEX(BELMONT_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = BELMONT_RED_BROWN_PURPLE,
            .location = {41.939751, -87.65338}
        },
        .led = {
            .start = RED_LINE_OFFSET + 48,
            .station = RED_LINE_OFFSET + 50,
            .count = 2
        }
    },
    [INDEX(BELMONT_KIMBALL_LINDEN_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = BELMONT_RED_BROWN_PURPLE,
            .location = {41.939751, -87.65338}
        }
    },
    [INDEX(BERWYN_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = BERWYN_RED,
            .location = {41.977984, -87.658668}
        },
        .led = {
            .start = RED_LINE_OFFSET + 57,
            .station = RED_LINE_OFFSET + 57,
            .count = 0
        }
    },
    [INDEX(BERWYN_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = BERWYN_RED,
            .location = {41.977984, -87.658668}
        },
        .led = {
            .start = RED_LINE_OFFSET + 57,
            .station = RED_LINE_OFFSET + 57,
            .count = 0
        }
    },
    [INDEX(BRYN_MAWR_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = BRYN_MAWR_RED,
            .location = {41.983504, -87.65884}
        },
        .led = {
            .start = RED_LINE_OFFSET + 58,
            .station = RED_LINE_OFFSET + 58,
            .count = 0
        }
    },
    [INDEX(BRYN_MAWR_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = BRYN_MAWR_RED,
            .location = {41.983504, -87.65884}
        },
        .led = {
            .start = RED_LINE_OFFSET + 58,
            .station = RED_LINE_OFFSET + 58,
            .count = 0
        }
    },
    [INDEX(CALIFORNIA_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CALIFORNIA_GREEN,
            .location = {41.88422, -87.696234}
        }
    },
    [INDEX(CALIFORNIA_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = CALIFORNIA_BLUE,
            .location = {41.921939, -87.69689}
        }
    },
    [INDEX(CALIFORNIA_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CALIFORNIA_GREEN,
            .location = {41.88422, -87.696234}
        }
    },
    [INDEX(CALIFORNIA_54TH_CERMAK_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = CALIFORNIA_PINK,
            .location = {41.854109, -87.694774}
        }
    },
    [INDEX(CALIFORNIA_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = CALIFORNIA_BLUE,
            .location = {41.921939, -87.69689}
        }
    },
    [INDEX(CALIFORNIA_LOOP_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = CALIFORNIA_PINK,
            .location = {41.854109, -87.694774}
        }
    },
    [INDEX(CENTRAL_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CENTRAL_GREEN,
            .location = {41.887389, -87.76565}
        }
    },
    [INDEX(CENTRAL_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CENTRAL_GREEN,
            .location = {41.887389, -87.76565}
        }
    },
    [INDEX(CENTRAL_LINDEN_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = CENTRAL_PURPLE,
            .location = {42.063987, -87.685617}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 1,
            .station = PURPLE_LINE_OFFSET + 1,
            .count = 0
        }
    },
    [INDEX(CENTRAL_HOWARD_LOOP_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = CENTRAL_PURPLE,
            .location = {42.063987, -87.685617}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 1,
            .station = PURPLE_LINE_OFFSET + 1,
            .count = 0
        }
    },
    [INDEX(CENTRAL_PARK_54TH_CERMAK_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = CENTRAL_PARK_PINK,
            .location = {41.853839, -87.714842}
        }
    },
    [INDEX(CENTRAL_PARK_LOOP_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = CENTRAL_PARK_PINK,
            .location = {41.853839, -87.714842}
        }
    },
    [INDEX(CERMAK_CHINATOWN_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = CERMAK_CHINATOWN_RED,
            .location = {41.853206, -87.630968}
        },
        .led = {
            .start = RED_LINE_OFFSET + 23,
            .station = RED_LINE_OFFSET + 26,
            .count = 3
        }
    },
    [INDEX(CERMAK_CHINATOWN_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = CERMAK_CHINATOWN_RED,
            .location = {41.853206, -87.630968}
        },
        .led = {
            .start = RED_LINE_OFFSET + 27,
            .station = RED_LINE_OFFSET + 26,
            .count = -1
        }
    },
    [INDEX(CERMAK_MCCORMICK_PLACE_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CERMAK_MCCORMICK_PLACE_GREEN,
            .location = {41.853115, -87.626402}
        }
    },
    [INDEX(CERMAK_MCCORMICK_PLACE_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CERMAK_MCCORMICK_PLACE_GREEN,
            .location = {41.853115, -87.626402}
        }
    },
    [INDEX(CHICAGO_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = CHICAGO_RED,
            .location = {41.896671, -87.628176}
        },
        .led = {
            .start = RED_LINE_OFFSET + 41,
            .station = RED_LINE_OFFSET + 39,
            .count = -2
        }
    },
    [INDEX(CHICAGO_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = CHICAGO_BROWN_PURPLE,
            .location = {41.89681, -87.635924}
        }
    },
    [INDEX(CHICAGO_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = CHICAGO_BLUE,
            .location = {41.896075, -87.655214}
        }
    },
    [INDEX(CHICAGO_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = CHICAGO_BLUE,
            .location = {41.896075, -87.655214}
        }
    },
    [INDEX(CHICAGO_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = CHICAGO_RED,
            .location = {41.896671, -87.628176}
        },
        .led = {
            .start = RED_LINE_OFFSET + 39,
            .station = RED_LINE_OFFSET + 39,
            .count = 0
        }
    },
    [INDEX(CHICAGO_KIMBALL_LINDEN_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = CHICAGO_BROWN_PURPLE,
            .location = {41.89681, -87.635924}
        }
    },
    [INDEX(CICERO_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = CICERO_BLUE,
            .location = {41.871574, -87.745154}
        }
    },
    [INDEX(CICERO_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CICERO_GREEN,
            .location = {41.886519, -87.744698}
        }
    },
    [INDEX(CICERO_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = CICERO_BLUE,
            .location = {41.871574, -87.745154}
        }
    },
    [INDEX(CICERO_LOOP_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = CICERO_PINK,
            .location = {41.85182, -87.745336}
        }
    },
    [INDEX(CICERO_54TH_CERMAK_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = CICERO_PINK,
            .location = {41.85182, -87.745336}
        }
    },
    [INDEX(CICERO_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CICERO_GREEN,
            .location = {41.886519, -87.744698}
        }
    },
    [INDEX(CLARK_DIVISION_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = CLARK_DIVISION_RED,
            .location = {41.90392, -87.631412}
        },
        .led = {
            .start = RED_LINE_OFFSET + 40,
            .station = RED_LINE_OFFSET + 42,
            .count = 2
        }
    },
    [INDEX(CLARK_DIVISION_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = CLARK_DIVISION_RED,
            .location = {41.90392, -87.631412}
        },
        .led = {
            .start = RED_LINE_OFFSET + 44,
            .station = RED_LINE_OFFSET + 42,
            .count = -2
        }
    },
    [INDEX(CLARK_LAKE_OUTER_LOOP)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CLARK_LAKE_BLUE_BROWN_GREEN_ORANGE_PURPLE_PINK,
            .location = {41.885737, -87.630886}
        }
    },
    [INDEX(CLARK_LAKE_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = CLARK_LAKE_BLUE_BROWN_GREEN_ORANGE_PURPLE_PINK,
            .location = {41.885737, -87.630886}
        }
    },
    [INDEX(CLARK_LAKE_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = CLARK_LAKE_BLUE_BROWN_GREEN_ORANGE_PURPLE_PINK,
            .location = {41.885737, -87.630886}
        }
    },
    [INDEX(CLARK_LAKE_INNER_LOOP)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CLARK_LAKE_BLUE_BROWN_GREEN_ORANGE_PURPLE_PINK,
            .location = {41.885737, -87.630886}
        }
    },
    [INDEX(CLINTON_LOOP_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CLINTON_GREEN_PINK,
            .location = {41.885678, -87.641782}
        }
    },
    [INDEX(CLINTON_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = CLINTON_BLUE,
            .location = {41.875539, -87.640984}
        }
    },
    [INDEX(CLINTON_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = CLINTON_BLUE,
            .location = {41.875539, -87.640984}
        }
    },
    [INDEX(CLINTON_HARLEM_54TH_CERMAK_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CLINTON_GREEN_PINK,
            .location = {41.885678, -87.641782}
        }
    },
    [INDEX(CONSERVATORY_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CONSERVATORY_GREEN,
            .location = {41.884904, -87.716523}
        }
    },
    [INDEX(CONSERVATORY_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = CONSERVATORY_GREEN,
            .location = {41.884904, -87.716523}
        }
    },
    [INDEX(COTTAGE_GROVE_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = COTTAGE_GROVE_GREEN,
            .location = {41.780309, -87.605857}
        }
    },
    [INDEX(COTTAGE_GROVE_TERMINAL_ARRIVAL)] = {
        .line = GREEN_LINE,
        .station = {
            .id = COTTAGE_GROVE_GREEN,
            .location = {41.780309, -87.605857}
        }
    },
    [INDEX(CUMBERLAND_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = CUMBERLAND_BLUE,
            .location = {41.984246, -87.838028}
        }
    },
    [INDEX(CUMBERLAND_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = CUMBERLAND_BLUE,
            .location = {41.984246, -87.838028}
        }
    },
    [INDEX(DAMEN_MILWAUKEE_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = DAMEN_BLUE,
            .location = {41.909744, -87.677437}
        }
    },
    [INDEX(DAMEN_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = DAMEN_GREEN,
            .location = {41.88497405, -87.67689149}
        }
    },
    [INDEX(DAMEN_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = DAMEN_GREEN,
            .location = {41.88497405, -87.67689149}
        }
    },
    [INDEX(DAMEN_54TH_CERMAK_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = DAMEN_PINK,
            .location = {41.854517, -87.675975}
        }
    },
    [INDEX(DAMEN_BROWN_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = DAMEN_BROWN,
            .location = {41.966286, -87.678639}
        }
    },
    [INDEX(DAMEN_MILWAUKEE_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = DAMEN_BLUE,
            .location = {41.909744, -87.677437}
        }
    },
    [INDEX(DAMEN_PINK_LOOP_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = DAMEN_PINK,
            .location = {41.854517, -87.675975}
        }
    },
    [INDEX(DAMEN_KIMBALL_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = DAMEN_BROWN,
            .location = {41.966286, -87.678639}
        }
    },
    [INDEX(DAVIS_LINDEN_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = DAVIS_PURPLE,
            .location = {42.04771, -87.683543}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 4,
            .station = PURPLE_LINE_OFFSET + 4,
            .count = 0
        }
    },
    [INDEX(DAVIS_HOWARD_LOOP_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = DAVIS_PURPLE,
            .location = {42.04771, -87.683543}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 4,
            .station = PURPLE_LINE_OFFSET + 4,
            .count = 0
        }
    },
    [INDEX(DEMPSTER_LINDEN_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = DEMPSTER_PURPLE,
            .location = {42.041655, -87.681602}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 5,
            .station = PURPLE_LINE_OFFSET + 5,
            .count = 0
        }
    },
    [INDEX(DEMPSTER_HOWARD_LOOP_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = DEMPSTER_PURPLE,
            .location = {42.041655, -87.681602}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 5,
            .station = PURPLE_LINE_OFFSET + 5,
            .count = 0
        }
    },
    [INDEX(DEMPSTER_SKOKIE_HOWARD_BOUND)] = {
        .line = YELLOW_LINE,
        .station = {
            .id = DEMPSTER_SKOKIE_YELLOW,
            .location = {42.038951, -87.751919}
        },
        .led = {
            .start = YELLOW_LINE_OFFSET + 11,
            .station = YELLOW_LINE_OFFSET + 11,
            .count = 0,
        }
    },
    [INDEX(DEMPSTER_SKOKIE_ARRIVAL)] = {
        .line = YELLOW_LINE,
        .station = {
            .id = DEMPSTER_SKOKIE_YELLOW,
            .location = {42.038951, -87.751919}
        },
        .led = {
            .start = YELLOW_LINE_OFFSET + 10,
            .station = YELLOW_LINE_OFFSET + 11,
            .count = 1,
        }
    },
    [INDEX(DIVERSEY_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = DIVERSEY_BROWN_PURPLE,
            .location = {41.932732, -87.653131}
        }
    },
    [INDEX(DIVERSEY_KIMBALL_LINDEN_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = DIVERSEY_BROWN_PURPLE,
            .location = {41.932732, -87.653131}
        }
    },
    [INDEX(DIVISION_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = DIVISION_BLUE,
            .location = {41.903355, -87.666496}
        }
    },
    [INDEX(DIVISION_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = DIVISION_BLUE,
            .location = {41.903355, -87.666496}
        }
    },
    [INDEX(FOREST_PARK_TERMINAL_ARRIVAL)] = {
        .line = BLUE_LINE,
        .station = {
            .id = FOREST_PARK_BLUE,
            .location = {41.874257, -87.817318}
        }
    },
    [INDEX(FOREST_PARK_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = FOREST_PARK_BLUE,
            .location = {41.874257, -87.817318}
        }
    },
    [INDEX(FOSTER_LINDEN_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = FOSTER_PURPLE,
            .location = {42.05416, -87.68356}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 3,
            .station = PURPLE_LINE_OFFSET + 3,
            .count = 0
        }
    },
    [INDEX(FOSTER_HOWARD_LOOP_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = FOSTER_PURPLE,
            .location = {42.05416, -87.68356}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 3,
            .station = PURPLE_LINE_OFFSET + 3,
            .count = 0
        }
    },
    [INDEX(FRANCISCO_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = FRANCISCO_BROWN,
            .location = {41.966046, -87.701644}
        }
    },
    [INDEX(FRANCISCO_KIMBALL_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = FRANCISCO_BROWN,
            .location = {41.966046, -87.701644}
        }
    },
    [INDEX(FULLERTON_KIMBALL_LINDEN_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = FULLERTON_RED_BROWN_PURPLE,
            .location = {41.925051, -87.652866}
        }
    },
    [INDEX(FULLERTON_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = FULLERTON_RED_BROWN_PURPLE,
            .location = {41.925051, -87.652866}
        },
        .led = {
            .start = RED_LINE_OFFSET + 46,
            .station = RED_LINE_OFFSET + 47,
            .count = 1
        }
    },
    [INDEX(FULLERTON_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = FULLERTON_RED_BROWN_PURPLE,
            .location = {41.925051, -87.652866}
        }
    },
    [INDEX(FULLERTON_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = FULLERTON_RED_BROWN_PURPLE,
            .location = {41.925051, -87.652866}
        },
        .led = {
            .start = RED_LINE_OFFSET + 49,
            .station = RED_LINE_OFFSET + 47,
            .count = -2
        }
    },
    [INDEX(GARFIELD_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = GARFIELD_RED,
            .location = {41.79542, -87.631157}
        },
        .led = {
            .start = RED_LINE_OFFSET + 17,
            .station = RED_LINE_OFFSET + 15,
            .count = -2
        }
    },
    [INDEX(GARFIELD_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = GARFIELD_RED,
            .location = {41.79542, -87.631157}
        },
        .led = {
            .start = RED_LINE_OFFSET + 12,
            .station = RED_LINE_OFFSET + 15,
            .count = 3
        }
    },
    [INDEX(GARFIELD_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = GARFIELD_GREEN,
            .location = {41.795172, -87.618327}
        }
    },
    [INDEX(GARFIELD_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = GARFIELD_GREEN,
            .location = {41.795172, -87.618327}
        }
    },
    [INDEX(GRAND_STATE_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = GRAND_RED,
            .location = {41.891665, -87.628021}
        },
        .led = {
            .start = RED_LINE_OFFSET + 38,
            .station = RED_LINE_OFFSET + 38,
            .count = 0
        }
    },
    [INDEX(GRAND_MILWAUKEE_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = GRAND_BLUE,
            .location = {41.891189, -87.647578}
        }
    },
    [INDEX(GRAND_MILWAUKEE_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = GRAND_BLUE,
            .location = {41.891189, -87.647578}
        }
    },
    [INDEX(GRAND_STATE_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = GRAND_RED,
            .location = {41.891665, -87.628021}
        },
        .led = {
            .start = RED_LINE_OFFSET + 37,
            .station = RED_LINE_OFFSET + 38,
            .count = 1
        }
    },
    [INDEX(GRANVILLE_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = GRANVILLE_RED,
            .location = {41.993664, -87.659202}
        },
        .led = {
            .start = RED_LINE_OFFSET + 60,
            .station = RED_LINE_OFFSET + 60,
            .count = 0
        }
    },
    [INDEX(GRANVILLE_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = GRANVILLE_RED,
            .location = {41.993664, -87.659202}
        },
        .led = {
            .start = RED_LINE_OFFSET + 60,
            .station = RED_LINE_OFFSET + 60,
            .count = 0
        }
    },
    [INDEX(HALSTED_63RD_ASHLAND_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = HALSTED_GREEN,
            .location = {41.778943, -87.644244}
        }
    },
    [INDEX(HALSTED_63RD_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = HALSTED_GREEN,
            .location = {41.778943, -87.644244}
        }
    },
    [INDEX(HALSTED_MIDWAY_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = HALSTED_ORANGE,
            .location = {41.84678, -87.648088}
        }
    },
    [INDEX(HALSTED_LOOP_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = HALSTED_ORANGE,
            .location = {41.84678, -87.648088}
        }
    },
    [INDEX(HARLEM_FOREST_PK_BRANCH_TERMINAL_ARRIVAL)] = {
        .line = BLUE_LINE,
        .station = {
            .id = HARLEM_BLUE_FOREST_PARK_BRANCH,
            .location = {41.87349, -87.806961}
        }
    },
    [INDEX(HARLEM_OHARE_BRANCH_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = HARLEM_BLUE_OHARE_BRANCH,
            .location = {41.98227, -87.8089}
        }
    },
    [INDEX(HARLEM_OHARE_BRANCH_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = HARLEM_BLUE_OHARE_BRANCH,
            .location = {41.98227, -87.8089}
        }
    },
    [INDEX(HARLEM_FOREST_PK_BRANCH_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = HARLEM_BLUE_FOREST_PARK_BRANCH,
            .location = {41.87349, -87.806961}
        }
    },
    [INDEX(HARLEM_TERMINAL_ARRIVAL)] = {
        .line = GREEN_LINE,
        .station = {
            .id = HARLEM_LAKE_GREEN,
            .location = {41.886848, -87.803176}
        }
    },
    [INDEX(HARLEM_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = HARLEM_LAKE_GREEN,
            .location = {41.886848, -87.803176}
        }
    },
    [INDEX(HW_LIBRARY_OUTER_LOOP)] = {
        .line = BROWN_LINE,
        .station = {
            .id = HAROLD_WASHINGTON_LIBRARY_STATE_VAN_BUREN_BROWN_ORANGE_PURPLE_PINK,
            .location = {41.876862, -87.628196}
        }
    },
    [INDEX(HW_LIBRARY_INNER_LOOP)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = HAROLD_WASHINGTON_LIBRARY_STATE_VAN_BUREN_BROWN_ORANGE_PURPLE_PINK,
            .location = {41.876862, -87.628196}
        }
    },
    [INDEX(HARRISON_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = HARRISON_RED,
            .location = {41.874039, -87.627479}
        },
        .led = {
            .start = RED_LINE_OFFSET + 31,
            .station = RED_LINE_OFFSET + 29,
            .count = -2
        }
    },
    [INDEX(HARRISON_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = HARRISON_RED,
            .location = {41.874039, -87.627479}
        },
        .led = {
            .start = RED_LINE_OFFSET + 29,
            .station = RED_LINE_OFFSET + 29,
            .count = 0
        }
    },
    [INDEX(HOWARD_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = HOWARD_RED_PURPLE_YELLOW,
            .location = {42.019063, -87.672892}
        },
        .led = {
            .start = RED_LINE_OFFSET + 64,
            .station = RED_LINE_OFFSET + 65,
            .count = 1
        }
    },
    [INDEX(HOWARD_LINDEN_SKOKIE_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = HOWARD_RED_PURPLE_YELLOW,
            .location = {42.019063, -87.672892}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 17,
            .station = PURPLE_LINE_OFFSET + 8,
            .count = -9
        }
    },
    [INDEX(HOWARD_RED_TERMINAL_ARRIVAL)] = {
        .line = RED_LINE,
        .station = {
            .id = HOWARD_RED_PURPLE_YELLOW,
            .location = {42.019063, -87.672892}
        },
        .led = {
            .start = RED_LINE_OFFSET + 65,
            .station = RED_LINE_OFFSET + 65,
            .count = 0
        }
    },
    [INDEX(HOWARD_PURPLE_YELLOW_TERMINAL_ARRIVAL)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = HOWARD_RED_PURPLE_YELLOW,
            .location = {42.019063, -87.672892}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 9,
            .station = PURPLE_LINE_OFFSET + 10,
            .count = 1
        }
    },
    [INDEX(HOWARD_YELLOW_SKOKIE_BOUND)] = {
        .line = YELLOW_LINE,
        .station = {
            .id = HOWARD_RED_PURPLE_YELLOW,
            .location = {42.019063, -87.672892}
        },
        .led = {
            .start = YELLOW_LINE_OFFSET,
            .station = YELLOW_LINE_OFFSET,
            .count = 0,
        }
    },
    [INDEX(HOWARD_YELLOW_TERMINAL_ARRIVAL)] = {
        .line = YELLOW_LINE,
        .station = {
            .id = HOWARD_RED_PURPLE_YELLOW,
            .location = {42.019063, -87.672892}
        },
        .led = {
            .start = YELLOW_LINE_OFFSET + 8,
            .station = YELLOW_LINE_OFFSET,
            .count = -8,
        }
    },
    [INDEX(ILLINOIS_MEDICAL_DISTRICT_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = ILLINOIS_MEDICAL_DISTRICT_BLUE,
            .location = {41.875706, -87.673932}
        }
    },
    [INDEX(ILLINOIS_MEDICAL_DISTRICT_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = ILLINOIS_MEDICAL_DISTRICT_BLUE,
            .location = {41.875706, -87.673932}
        }
    },
    [INDEX(INDIANA_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = INDIANA_GREEN,
            .location = {41.821732, -87.621371}
        }
    },
    [INDEX(INDIANA_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = INDIANA_GREEN,
            .location = {41.821732, -87.621371}
        }
    },
    [INDEX(IRVING_PARK_OHARE_BRANCH_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = IRVING_PARK_BLUE,
            .location = {41.952925, -87.729229}
        }
    },
    [INDEX(IRVING_PARK_KIMBALL_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = IRVING_PARK_BROWN,
            .location = {41.954521, -87.674868}
        }
    },
    [INDEX(IRVING_PARK_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = IRVING_PARK_BROWN,
            .location = {41.954521, -87.674868}
        }
    },
    [INDEX(IRVING_PARK_OHARE_BRANCH_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = IRVING_PARK_BLUE,
            .location = {41.952925, -87.729229}
        }
    },
    [INDEX(JACKSON_DEARBORN_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = JACKSON_BLUE,
            .location = {41.878183, -87.629296}
        }
    },
    [INDEX(JACKSON_STATE_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = JACKSON_RED,
            .location = {41.878153, -87.627596}
        },
        .led = {
            .start = RED_LINE_OFFSET + 30,
            .station = RED_LINE_OFFSET + 32,
            .count = 2
        }
    },
    [INDEX(JACKSON_STATE_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = JACKSON_RED,
            .location = {41.878153, -87.627596}
        },
        .led = {
            .start = RED_LINE_OFFSET + 32,
            .station = RED_LINE_OFFSET + 32,
            .count = 0
        }
    },
    [INDEX(JACKSON_DEARBORN_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = JACKSON_BLUE,
            .location = {41.878183, -87.629296}
        }
    },
    [INDEX(JARVIS_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = JARVIS_RED,
            .location = {42.015876, -87.669092}
        },
        .led = {
            .start = RED_LINE_OFFSET + 64,
            .station = RED_LINE_OFFSET + 63,
            .count = -1
        }
    },
    [INDEX(JARVIS_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = JARVIS_RED,
            .location = {42.015876, -87.669092}
        },
        .led = {
            .start = RED_LINE_OFFSET + 63,
            .station = RED_LINE_OFFSET + 63,
            .count = 0
        }
    },
    [INDEX(JEFFERSON_PARK_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = JEFFERSON_PARK_BLUE,
            .location = {41.970634, -87.760892}
        }
    },
    [INDEX(JEFFERSON_PARK_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = JEFFERSON_PARK_BLUE,
            .location = {41.970634, -87.760892}
        }
    },
    [INDEX(KEDZIE_ORANGE_LOOP_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = KEDZIE_ORANGE,
            .location = {41.804236, -87.704406}
        }
    },
    [INDEX(KEDZIE_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = KEDZIE_GREEN,
            .location = {41.884321, -87.706155}
        }
    },
    [INDEX(KEDZIE_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = KEDZIE_GREEN,
            .location = {41.884321, -87.706155}
        }
    },
    [INDEX(KEDZIE_MIDWAY_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = KEDZIE_ORANGE,
            .location = {41.804236, -87.704406}
        }
    },
    [INDEX(KEDZIE_54TH_CERMAK_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = KEDZIE_PINK,
            .location = {41.853964, -87.705408}
        }
    },
    [INDEX(KEDZIE_PINK_LOOP_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = KEDZIE_PINK,
            .location = {41.853964, -87.705408}
        }
    },
    [INDEX(KEDZIE_KIMBALL_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = KEDZIE_BROWN,
            .location = {41.965996, -87.708821}
        }
    },
    [INDEX(KEDZIE_BROWN_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = KEDZIE_BROWN,
            .location = {41.965996, -87.708821}
        }
    },
    [INDEX(KEDZIE_HOMAN_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = KEDZIE_HOMAN_BLUE,
            .location = {41.874341, -87.70604}
        }
    },
    [INDEX(KEDZIE_HOMAN_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = KEDZIE_HOMAN_BLUE,
            .location = {41.874341, -87.70604}
        }
    },
    [INDEX(KIMBALL_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = KIMBALL_BROWN,
            .location = {41.967901, -87.713065}
        }
    },
    [INDEX(KIMBALL_TERMINAL_ARRIVAL)] = {
        .line = BROWN_LINE,
        .station = {
            .id = KIMBALL_BROWN,
            .location = {41.967901, -87.713065}
        }
    },
    [INDEX(KING_DRIVE_COTTAGE_GROVE_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = KING_DRIVE_GREEN,
            .location = {41.78013, -87.615546}
        }
    },
    [INDEX(KING_DRIVE_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = KING_DRIVE_GREEN,
            .location = {41.78013, -87.615546}
        }
    },
    [INDEX(KOSTNER_LOOP_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = KOSTNER_PINK,
            .location = {41.853751, -87.733258}
        }
    },
    [INDEX(KOSTNER_54TH_CERMAK_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = KOSTNER_PINK,
            .location = {41.853751, -87.733258}
        }
    },
    [INDEX(LAKE_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = LAKE_RED,
            .location = {41.884809, -87.627813}
        },
        .led = {
            .start = RED_LINE_OFFSET + 37,
            .station = RED_LINE_OFFSET + 36,
            .count = -1
        }
    },
    [INDEX(LAKE_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = LAKE_RED,
            .location = {41.884809, -87.627813}
        },
        .led = {
            .start = RED_LINE_OFFSET + 34,
            .station = RED_LINE_OFFSET + 36,
            .count = 2
        }
    },
    [INDEX(LARAMIE_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = LARAMIE_GREEN,
            .location = {41.887163, -87.754986}
        }
    },
    [INDEX(LARAMIE_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = LARAMIE_GREEN,
            .location = {41.887163, -87.754986}
        }
    },
    [INDEX(LASALLE_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = LASALLE_BLUE,
            .location = {41.875568, -87.631722}
        }
    },
    [INDEX(LASALLE_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = LASALLE_BLUE,
            .location = {41.875568, -87.631722}
        }
    },
    [INDEX(LASALLE_VAN_BUREN_OUTER_LOOP)] = {
        .line = BROWN_LINE,
        .station = {
            .id = LASALLE_VAN_BUREN_BROWN_ORANGE_PURPLE_PINK,
            .location = {41.8768, -87.631739}
        }
    },
    [INDEX(LASALLE_VAN_BUREN_INNER_LOOP)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = LASALLE_VAN_BUREN_BROWN_ORANGE_PURPLE_PINK,
            .location = {41.8768, -87.631739}
        }
    },
    [INDEX(LAWRENCE_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = LAWRENCE_RED,
            .location = {41.969139, -87.658493}
        },
        .led = {
            .start = RED_LINE_OFFSET + 55,
            .station = RED_LINE_OFFSET + 55,
            .count = 0
        }
    },
    [INDEX(LAWRENCE_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = LAWRENCE_RED,
            .location = {41.969139, -87.658493}
        },
        .led = {
            .start = RED_LINE_OFFSET + 55,
            .station = RED_LINE_OFFSET + 55,
            .count = 0
        }
    },
    [INDEX(LINDEN_LINDEN_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = LINDEN_PURPLE,
            .location = {42.073153, -87.69073}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET,
            .station = PURPLE_LINE_OFFSET,
            .count = 0
        }
    },
    [INDEX(LINDEN_HOWARD_LOOP_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = LINDEN_PURPLE,
            .location = {42.073153, -87.69073}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET,
            .station = PURPLE_LINE_OFFSET,
            .count = 0
        }
    },
    [INDEX(LOGAN_SQUARE_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = LOGAN_SQUARE_BLUE,
            .location = {41.929728, -87.708541}
        }
    },
    [INDEX(LOGAN_SQUARE_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = LOGAN_SQUARE_BLUE,
            .location = {41.929728, -87.708541}
        }
    },
    [INDEX(LOYOLA_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = LOYOLA_RED,
            .location = {42.001073, -87.661061}
        },
        .led = {
            .start = RED_LINE_OFFSET + 61,
            .station = RED_LINE_OFFSET + 61,
            .count = 0
        }
    },
    [INDEX(LOYOLA_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = LOYOLA_RED,
            .location = {42.001073, -87.661061}
        },
        .led = {
            .start = RED_LINE_OFFSET + 61,
            .station = RED_LINE_OFFSET + 61,
            .count = 0
        }
    },
    [INDEX(MAIN_LINDEN_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = MAIN_PURPLE,
            .location = {42.033456, -87.679538}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 6,
            .station = PURPLE_LINE_OFFSET + 6,
            .count = 0
        }
    },
    [INDEX(MAIN_HOWARD_LOOP_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = MAIN_PURPLE,
            .location = {42.033456, -87.679538}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 6,
            .station = PURPLE_LINE_OFFSET + 6,
            .count = 0
        }
    },
    [INDEX(MERCHANDISE_MART_KIMBALL_LINDEN_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = MERCHANDISE_MART_BROWN_PURPLE,
            .location = {41.888969, -87.633924}
        }
    },
    [INDEX(MERCHANDISE_MART_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = MERCHANDISE_MART_BROWN_PURPLE,
            .location = {41.888969, -87.633924}
        }
    },
    [INDEX(MIDWAY_ARRIVAL)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = MIDWAY_ORANGE,
            .location = {41.78661, -87.737875}
        }
    },
    [INDEX(MIDWAY_LOOP_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = MIDWAY_ORANGE,
            .location = {41.78661, -87.737875}
        }
    },
    [INDEX(MONROE_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = MONROE_RED,
            .location = {41.880745, -87.627696}
        },
        .led = {
            .start = RED_LINE_OFFSET + 35,
            .station = RED_LINE_OFFSET + 33,
            .count = -2
        }
    },
    [INDEX(MONROE_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = MONROE_RED,
            .location = {41.880745, -87.627696}
        },
        .led = {
            .start = RED_LINE_OFFSET + 33,
            .station = RED_LINE_OFFSET + 33,
            .count = 0
        }
    },
    [INDEX(MONROE_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = MONROE_BLUE,
            .location = {41.880703, -87.629378}
        }
    },
    [INDEX(MONROE_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = MONROE_BLUE,
            .location = {41.880703, -87.629378}
        }
    },
    [INDEX(MONTROSE_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = MONTROSE_BLUE,
            .location = {41.961539, -87.743574}
        }
    },
    [INDEX(MONTROSE_KIMBALL_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = MONTROSE_BROWN,
            .location = {41.961756, -87.675047}
        }
    },
    [INDEX(MONTROSE_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = MONTROSE_BROWN,
            .location = {41.961756, -87.675047}
        }
    },
    [INDEX(MONTROSE_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = MONTROSE_BLUE,
            .location = {41.961539, -87.743574}
        }
    },
    [INDEX(MORGAN_HARLEM_54TH_CERMAK_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = MORGAN_GREEN_PINK,
            .location = {41.885586, -87.652193}
        }
    },
    [INDEX(MORGAN_LOOP_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = MORGAN_GREEN_PINK,
            .location = {41.885586, -87.652193}
        }
    },
    [INDEX(MORSE_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = MORSE_RED,
            .location = {42.008362, -87.665909}
        },
        .led = {
            .start = RED_LINE_OFFSET + 62,
            .station = RED_LINE_OFFSET + 62,
            .count = 0
        }
    },
    [INDEX(MORSE_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = MORSE_RED,
            .location = {42.008362, -87.665909}
        },
        .led = {
            .start = RED_LINE_OFFSET + 62,
            .station = RED_LINE_OFFSET + 62,
            .count = 0
        }
    },
    [INDEX(NORTH_CLYBOURN_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = NORTH_CLYBOURN_RED,
            .location = {41.910655, -87.649177}
        },
        .led = {
            .start = RED_LINE_OFFSET + 46,
            .station = RED_LINE_OFFSET + 45,
            .count = -1
        }
    },
    [INDEX(NORTH_CLYBOURN_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = NORTH_CLYBOURN_RED,
            .location = {41.910655, -87.649177}
        },
        .led = {
            .start = RED_LINE_OFFSET + 43,
            .station = RED_LINE_OFFSET + 45,
            .count = 2
        }
    },
    [INDEX(NOYES_HOWARD_LOOP_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = NOYES_PURPLE,
            .location = {42.058282, -87.683337}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 2,
            .station = PURPLE_LINE_OFFSET + 2,
            .count = 0
        }
    },
    [INDEX(NOYES_LINDEN_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = NOYES_PURPLE,
            .location = {42.058282, -87.683337}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 2,
            .station = PURPLE_LINE_OFFSET + 2,
            .count = 0
        }
    },
    [INDEX(OAK_PARK_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = OAK_PARK_GREEN,
            .location = {41.886988, -87.793783}
        }
    },
    [INDEX(OAK_PARK_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = OAK_PARK_BLUE,
            .location = {41.872108, -87.791602}
        }
    },
    [INDEX(OAK_PARK_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = OAK_PARK_BLUE,
            .location = {41.872108, -87.791602}
        }
    },
    [INDEX(OAK_PARK_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = OAK_PARK_GREEN,
            .location = {41.886988, -87.793783}
        }
    },
    [INDEX(OAKTON_SKOKIE_DEMPSTER_SKOKIE_BOUND)] = {
        .line = YELLOW_LINE,
        .station = {
            .id = OAKTON_SKOKIE_YELLOW,
            .location = {42.02624348, -87.74722084}
        },
        .led = {
            .start = YELLOW_LINE_OFFSET + 1,
            .station = YELLOW_LINE_OFFSET + 9,
            .count = 8,
        }
    },
    [INDEX(OAKTON_SKOKIE_HOWARD_BOUND)] = {
        .line = YELLOW_LINE,
        .station = {
            .id = OAKTON_SKOKIE_YELLOW,
            .location = {42.02624348, -87.74722084}
        },
        .led = {
            .start = YELLOW_LINE_OFFSET + 10,
            .station = YELLOW_LINE_OFFSET + 9,
            .count = -1,
        }
    },
    [INDEX(OHARE_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = OHARE_BLUE,
            .location = {41.97766526, -87.90422307}
        }
    },
    [INDEX(OHARE_TERMINAL_ARRIVAL)] = {
        .line = BLUE_LINE,
        .station = {
            .id = OHARE_BLUE,
            .location = {41.97766526, -87.90422307}
        }
    },
    [INDEX(PAULINA_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = PAULINA_BROWN,
            .location = {41.943623, -87.670907}
        }
    },
    [INDEX(PAULINA_KIMBALL_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = PAULINA_BROWN,
            .location = {41.943623, -87.670907}
        }
    },
    [INDEX(POLK_LOOP_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = POLK_PINK,
            .location = {41.871551, -87.66953}
        }
    },
    [INDEX(POLK_54TH_CERMAK_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = POLK_PINK,
            .location = {41.871551, -87.66953}
        }
    },
    [INDEX(PULASKI_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = PULASKI_GREEN,
            .location = {41.885412, -87.725404}
        }
    },
    [INDEX(PULASKI_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = PULASKI_BLUE,
            .location = {41.873797, -87.725663}
        }
    },
    [INDEX(PULASKI_PINK_LOOP_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = PULASKI_PINK,
            .location = {41.853732, -87.724311}
        }
    },
    [INDEX(PULASKI_MIDWAY_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = PULASKI_ORANGE,
            .location = {41.799756, -87.724493}
        }
    },
    [INDEX(PULASKI_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = PULASKI_BLUE,
            .location = {41.873797, -87.725663}
        }
    },
    [INDEX(PULASKI_ORANGE_LOOP_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = PULASKI_ORANGE,
            .location = {41.799756, -87.724493}
        }
    },
    [INDEX(PULASKI_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = PULASKI_GREEN,
            .location = {41.885412, -87.725404}
        }
    },
    [INDEX(PULASKI_54TH_CERMAK_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = PULASKI_PINK,
            .location = {41.853732, -87.724311}
        }
    },
    [INDEX(QUINCY_WELLS_INNER_LOOP)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = QUINCY_WELLS_BROWN_ORANGE_PURPLE_PINK,
            .location = {41.878723, -87.63374}
        }
    },
    [INDEX(QUINCY_WELLS_OUTER_LOOP)] = {
        .line = BROWN_LINE,
        .station = {
            .id = QUINCY_WELLS_BROWN_ORANGE_PURPLE_PINK,
            .location = {41.878723, -87.63374}
        }
    },
    [INDEX(RACINE_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = RACINE_BLUE,
            .location = {41.87592, -87.659458}
        }
    },
    [INDEX(RACINE_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = RACINE_BLUE,
            .location = {41.87592, -87.659458}
        }
    },
    [INDEX(RIDGELAND_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = RIDGELAND_GREEN,
            .location = {41.887159, -87.783661}
        }
    },
    [INDEX(RIDGELAND_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = RIDGELAND_GREEN,
            .location = {41.887159, -87.783661}
        }
    },
    [INDEX(ROCKWELL_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = ROCKWELL_BROWN,
            .location = {41.966115, -87.6941}
        }
    },
    [INDEX(ROCKWELL_KIMBALL_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = ROCKWELL_BROWN,
            .location = {41.966115, -87.6941}
        }
    },
    [INDEX(ROOSEVELT_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = ROOSEVELT_RED_ORANGE_GREEN,
            .location = {41.867368, -87.627402}
        },
        .led = {
            .start = RED_LINE_OFFSET + 27,
            .station = RED_LINE_OFFSET + 28,
            .count = 1
        }
    },
    [INDEX(ROOSEVELT_MIDWAY_63RD_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = ROOSEVELT_RED_ORANGE_GREEN,
            .location = {41.867405, -87.62659}
        }
    },
    [INDEX(ROOSEVELT_LOOP_HARLEM_BOUND)] = {
        .line = GREEN_LINE,
        .station = {
            .id = ROOSEVELT_RED_ORANGE_GREEN,
            .location = {41.867405, -87.62659}
        }
    },
    [INDEX(ROOSEVELT_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = ROOSEVELT_RED_ORANGE_GREEN,
            .location = {41.867368, -87.627402}
        },
        .led = {
            .start = RED_LINE_OFFSET + 28,
            .station = RED_LINE_OFFSET + 28,
            .count = 0
        }
    },
    [INDEX(ROSEMONT_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = ROSEMONT_BLUE,
            .location = {41.983507, -87.859388}
        }
    },
    [INDEX(ROSEMONT_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = ROSEMONT_BLUE,
            .location = {41.983507, -87.859388}
        }
    },
    [INDEX(SEDGWICK_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = SEDGWICK_BROWN_PURPLE,
            .location = {41.910409, -87.639302}
        }
    },
    [INDEX(SEDGWICK_KIMBALL_LINDEN_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = SEDGWICK_BROWN_PURPLE,
            .location = {41.910409, -87.639302}
        }
    },
    [INDEX(SHERIDAN_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = SHERIDAN_RED,
            .location = {41.953775, -87.654929}
        },
        .led = {
            .start = RED_LINE_OFFSET + 53,
            .station = RED_LINE_OFFSET + 53,
            .count = 0
        }
    },
    [INDEX(SHERIDAN_LOOP_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = SHERIDAN_RED,
            .location = {41.953775, -87.654929}
        },
        .led = {
            .start = RED_LINE_OFFSET + 53,
            .station = RED_LINE_OFFSET + 53,
            .count = 0
        }
    },
    [INDEX(SHERIDAN_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = SHERIDAN_RED,
            .location = {41.953775, -87.654929}
        },
        .led = {
            .start = RED_LINE_OFFSET + 53,
            .station = RED_LINE_OFFSET + 53,
            .count = 0
        }
    },
    [INDEX(SHERIDAN_HOWARD_LINDEN_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = SHERIDAN_RED,
            .location = {41.953775, -87.654929}
        },
        .led = {
            .start = RED_LINE_OFFSET + 53,
            .station = RED_LINE_OFFSET + 53,
            .count = 0
        }
    },
    [INDEX(SOUTH_BLVD_HOWARD_LOOP_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = SOUTH_BOULEVARD_PURPLE,
            .location = {42.027612, -87.678329}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 7,
            .station = PURPLE_LINE_OFFSET + 7,
            .count = 0
        }
    },
    [INDEX(SOUTH_BLVD_LINDEN_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = SOUTH_BOULEVARD_PURPLE,
            .location = {42.027612, -87.678329}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 9,
            .station = PURPLE_LINE_OFFSET + 8,
            .count = -1
        }
    },
    [INDEX(SOUTHPORT_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = SOUTHPORT_BROWN,
            .location = {41.943744, -87.663619}
        }
    },
    [INDEX(SOUTHPORT_KIMBALL_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = SOUTHPORT_BROWN,
            .location = {41.943744, -87.663619}
        }
    },
    [INDEX(SOX_35TH_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = SOX_35TH_RED,
            .location = {41.831191, -87.630636}
        },
        .led = {
            .start = RED_LINE_OFFSET + 19,
            .station = RED_LINE_OFFSET + 22,
            .count = 3
        }
    },
    [INDEX(SOX_35TH_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = SOX_35TH_RED,
            .location = {41.831191, -87.630636}
        },
        .led = {
            .start = RED_LINE_OFFSET + 25,
            .station = RED_LINE_OFFSET + 22,
            .count = -3
        }
    },
    [INDEX(STATE_LAKE_INNER_LOOP)] = {
        .line = GREEN_LINE,
        .station = {
            .id = STATE_LAKE_BROWN_GREEN_ORANGE_PINK_PURPLE,
            .location = {41.88574, -87.627835}
        }
    },
    [INDEX(STATE_LAKE_OUTER_LOOP)] = {
        .line = GREEN_LINE,
        .station = {
            .id = STATE_LAKE_BROWN_GREEN_ORANGE_PINK_PURPLE,
            .location = {41.88574, -87.627835}
        }
    },
    [INDEX(THORNDALE_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = THORNDALE_RED,
            .location = {41.990259, -87.659076}
        },
        .led = {
            .start = RED_LINE_OFFSET + 59,
            .station = RED_LINE_OFFSET + 59,
            .count = 0
        }
    },
    [INDEX(THORNDALE_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = THORNDALE_RED,
            .location = {41.990259, -87.659076}
        },
        .led = {
            .start = RED_LINE_OFFSET + 59,
            .station = RED_LINE_OFFSET + 59,
            .count = 0
        }
    },
    [INDEX(UIC_HALSTED_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = UIC_HALSTED_BLUE,
            .location = {41.875474, -87.649707}
        }
    },
    [INDEX(UIC_HALSTED_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = UIC_HALSTED_BLUE,
            .location = {41.875474, -87.649707}
        }
    },
    [INDEX(WASHINGTON_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = WASHINGTON_BLUE,
            .location = {41.883164, -87.62944}
        }
    },
    [INDEX(WASHINGTON_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = WASHINGTON_BLUE,
            .location = {41.883164, -87.62944}
        }
    },
    [INDEX(WASHINGTON_WABASH_INNER_LOOP)] = {
        .line = GREEN_LINE,
        .station = {
            .id = WASHINGTON_WABASH_BROWN_GREEN_ORANGE_PURPLE_PINK,
            .location = {41.88322, -87.626189}
        }
    },
    [INDEX(WASHINGTON_WABASH_OUTER_LOOP)] = {
        .line = GREEN_LINE,
        .station = {
            .id = WASHINGTON_WABASH_BROWN_GREEN_ORANGE_PURPLE_PINK,
            .location = {41.88322, -87.626189}
        }
    },
    [INDEX(WASHINGTON_WELLS_OUTER_LOOP)] = {
        .line = BROWN_LINE,
        .station = {
            .id = WASHINGTON_WELLS_BROWN_ORANGE_PURPLE_PINK,
            .location = {41.882695, -87.63378}
        }
    },
    [INDEX(WASHINGTON_WELLS_INNER_LOOP)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = WASHINGTON_WELLS_BROWN_ORANGE_PURPLE_PINK,
            .location = {41.882695, -87.63378}
        }
    },
    [INDEX(WELLINGTON_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = WELLINGTON_BROWN_PURPLE,
            .location = {41.936033, -87.653266}
        }
    },
    [INDEX(WELLINGTON_KIMBALL_LINDEN_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = WELLINGTON_BROWN_PURPLE,
            .location = {41.936033, -87.653266}
        }
    },
    [INDEX(WESTERN_BROWN_LOOP_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = WESTERN_BROWN,
            .location = {41.966163, -87.688502}
        }
    },
    [INDEX(WESTERN_OHARE_BRANCH_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = WESTERN_BLUE_OHARE_BRANCH,
            .location = {41.916157, -87.687364}
        }
    },
    [INDEX(WESTERN_FOREST_PK_BRANCH_OHARE_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = WESTERN_BLUE_FOREST_PARK_BRANCH,
            .location = {41.875478, -87.688436}
        }
    },
    [INDEX(WESTERN_MIDWAY_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = WESTERN_ORANGE,
            .location = {41.804546, -87.684019}
        }
    },
    [INDEX(WESTERN_54TH_CERMAK_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = WESTERN_PINK,
            .location = {41.854225, -87.685129}
        }
    },
    [INDEX(WESTERN_FOREST_PK_BRANCH_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = WESTERN_BLUE_FOREST_PARK_BRANCH,
            .location = {41.875478, -87.688436}
        }
    },
    [INDEX(WESTERN_ORANGE_LOOP_BOUND)] = {
        .line = ORANGE_LINE,
        .station = {
            .id = WESTERN_ORANGE,
            .location = {41.804546, -87.684019}
        }
    },
    [INDEX(WESTERN_OHARE_BRANCH_FOREST_PK_BOUND)] = {
        .line = BLUE_LINE,
        .station = {
            .id = WESTERN_BLUE_OHARE_BRANCH,
            .location = {41.916157, -87.687364}
        }
    },
    [INDEX(WESTERN_KIMBALL_BOUND)] = {
        .line = BROWN_LINE,
        .station = {
            .id = WESTERN_BROWN,
            .location = {41.966163, -87.688502}
        }
    },
    [INDEX(WESTERN_PINK_LOOP_BOUND)] = {
        .line = PINK_LINE,
        .station = {
            .id = WESTERN_PINK,
            .location = {41.854225, -87.685129}
        }
    },
    [INDEX(WILSON_LOOP_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = WILSON_RED_PURPLE,
            .location = {41.964273, -87.657588}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 9,
            .station = PURPLE_LINE_OFFSET + 18,
            .count = 9
        }
    },
    [INDEX(WILSON_LINDEN_BOUND)] = {
        .line = PURPLE_LINE,
        .station = {
            .id = WILSON_RED_PURPLE,
            .location = {41.964273, -87.657588}
        },
        .led = {
            .start = PURPLE_LINE_OFFSET + 21,
            .station = PURPLE_LINE_OFFSET + 18,
            .count = -3
        }
    },
    [INDEX(WILSON_95TH_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = WILSON_RED_PURPLE,
            .location = {41.964273, -87.657588}
        },
        .led = {
            .start = RED_LINE_OFFSET + 54,
            .station = RED_LINE_OFFSET + 54,
            .count = 0
        }
    },
    [INDEX(WILSON_HOWARD_BOUND)] = {
        .line = RED_LINE,
        .station = {
            .id = WILSON_RED_PURPLE,
            .location = {41.964273, -87.657588}
        },
        .led = {
            .start = RED_LINE_OFFSET + 54,
            .station = RED_LINE_OFFSET + 54,
            .count = 0
        }
    }
};

led_index_t cta_get_led_index(size_t stop_id)
{
    return stops[INDEX(stop_id)].led;
}
