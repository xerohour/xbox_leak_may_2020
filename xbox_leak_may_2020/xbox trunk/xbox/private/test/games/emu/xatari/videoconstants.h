#ifndef _VIDEOCONSTANTS_H_
#define _VIDEOCONSTANTS_H_

#define TITLE_NAME      L"Atari 2600"
#define FOOTER_TEXT     L"D: Play Game  |  7: Prev Screen  |  8: Next Screen"

#define SCREEN_WIDTH                    640
#define SCREEN_HEIGHT                   480
#define SCREEN_X_CENTER                 SCREEN_WIDTH / 2
#define SCREEN_Y_CENTER                 SCREEN_HEIGHT / 2

// Locations
#define HEADER_BOTTOM_Y_POSITION        35.0f
#define HEADER_LINE_Y_POSITION          HEADER_BOTTOM_Y_POSITION
#define HEADER_TEXT_X_POSITION          10.0f
#define FOOTER_TOP_Y_POSITION           SCREEN_HEIGHT - 35.0f
#define FOOTER_LINE_Y_POSITION          FOOTER_TOP_Y_POSITION
#define FOOTER_TEXT_X_POSITION          10.0f
#define MENU_TITLE_Y                    50.0f
#define MENU_OPTION1_Y                  52.0f
#define MENU_ROW1_Y                     15.0f
#define MENU_ROW2_Y                     -15.0f
#define MENU_COL1_X                     -20.0f
#define MENU_COL2_X                     20.0f
#define MENU_HIGHLIGHT_BORDER_SPACE     2
#define MENU_HIGHLIGHT_BORDER_WIDTH     2
#define MENU_VERT_SPACING               10.0f
#define MENU_HORIZ_SPACING				15.0f
#define MENU_NUM_COL					5
#define MENU_NUM_ROW					15
#define HEADER_LINE_WIDTH               2
#define FOOTER_LINE_WIDTH               2

// Colors
#define WHITE                           0xFFFFFFFF
#define BLACK                           0xFF000000
#define RED                             0xFFFF0000
#define LIGHT_GREEN                     0xFF90EE90
#define GREEN                           0xFF00FF00
#define LIGHT_BLUE                      0xFFADD8E6
#define NAVY_BLUE                       0xFF000080
#define BLUE                            0xFF0000FF
#define MEDIUM_BLUE                     0xFF0000CD
#define YELLOW                          0xFFFFFF00
#define CYAN                            0xFF00FFFF
#define BROWN                           0xFFA52A2A
#define HOT_PINK                        0xFFFF69B4

#define BACKGROUND_COLOR                BLACK
#define DEFAULT_TEXT_FG_COLOR           WHITE
#define DEFAULT_TEXT_BG_COLOR           BACKGROUND_COLOR

#define MENU_TITLE_FG_COLOR             BLUE
#define MENU_TITLE_BG_COLOR             DEFAULT_TEXT_BG_COLOR
#define MENU_ITEM_FG_COLOR              DEFAULT_TEXT_FG_COLOR
#define MENU_ITEM_BG_COLOR              DEFAULT_TEXT_BG_COLOR
#define HEADER_TEXT_FG_COLOR            DEFAULT_TEXT_FG_COLOR
#define HEADER_TEXT_BG_COLOR            DEFAULT_TEXT_BG_COLOR
#define HEADER_BG_COLOR                 NAVY_BLUE
#define HEADER_LINE_COLOR               RED
#define FOOTER_TEXT_FG_COLOR            DEFAULT_TEXT_FG_COLOR
#define FOOTER_TEXT_BG_COLOR            DEFAULT_TEXT_BG_COLOR
#define FOOTER_BG_COLOR                 NAVY_BLUE
#define FOOTER_LINE_COLOR               RED
#define HIGHLIGHT_BOX_COLOR             YELLOW

#endif // _VIDEOCONSTANTS_H_