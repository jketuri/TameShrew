
#ifndef _js_Garmin_hpp_
#define _js_Garmin_hpp_

#include <stdlib.h>
#include <iostream>
#include <iosfwd>
#include <map>
#include <sstream>
#include <streambuf>
#include <string>
#include <utility>
#include <vector>
#include "common.hpp"

typedef unsigned char uint8;

typedef unsigned short uint16;

typedef unsigned int uint32;

typedef signed short sint16;

typedef signed long sint32;

typedef float float32;

typedef double float64;

#pragma pack( push, 1)

typedef struct {
    unsigned char  mPacketType;
    unsigned char  mReserved1;
    unsigned short mReserved2;
    unsigned short mPacketId;
    unsigned short mReserved3;
    unsigned long  mDataSize;
} Packet;

enum PacketType {
    Pt_USBProtocolLayer = 0,
    Pt_ApplicationLayer = 20
};

enum USBProtocolPacketID {
    Pid_Data_Available = 2,
    Pid_Start_Session = 5,
    Pid_Session_Started = 6
};

enum BasicPacketID {
    Pid_L000_Protocol_Array = 253, /* may not be implemented in all devices */
    Pid_L000_Product_Rqst = 254,
    Pid_L000_Product_Data = 255,
    Pid_L000_Ext_Product_Data = 248 /* may not be implemented in all devices */
};

enum LinkProtocol1 {
    Pid_L001_Command_Data = 10,
    Pid_L001_Xfer_Cmplt = 12,
    Pid_L001_Date_Time_Data = 14,
    Pid_L001_Position_Data = 17,
    Pid_L001_Prx_Wpt_Data = 19,
    Pid_L001_Records = 27,
    Pid_L001_Rte_Hdr = 29,
    Pid_L001_Rte_Wpt_Data = 30,
    Pid_L001_Almanac_Data = 31,
    Pid_L001_Trk_Data = 34,
    Pid_L001_Wpt_Data = 35,
    Pid_L001_Pvt_Data = 51,
    Pid_L001_Rte_Link_Data = 98,
    Pid_L001_Trk_Hdr = 99,
    Pid_L001_FlightBook_Record = 134,
    Pid_L001_Lap = 149,
    Pid_L001_Wpt_Cat = 152,
    Pid_L001_Run = 990,
    Pid_L001_Workout = 991,
    Pid_L001_Workout_Occurrence = 992,
    Pid_L001_Fitness_User_Profile = 993,
    Pid_L001_Workout_Limits = 994,
    Pid_L001_Course = 1061,
    Pid_L001_Course_Lap = 1062,
    Pid_L001_Course_Point = 1063,
    Pid_L001_Course_Trk_Hdr = 1064,
    Pid_L001_Course_Trk_Data = 1065,
    Pid_L001_Course_Limits = 1066
};

enum LinkProtocol2 {
    Pid_L002_Almanac_Data = 4,
    Pid_L002_Command_Data = 11,
    Pid_L002_Xfer_Cmplt = 12,
    Pid_L002_Date_Time_Data = 20,
    Pid_L002_Position_Data = 24,
    Pid_L002_Prx_Wpt_Data = 27,
    Pid_L002_Records = 35,
    Pid_L002_Rte_Hdr = 37,
    Pid_L002_Rte_Wpt_Data = 39,
    Pid_L002_Wpt_Data = 43
};

typedef uint16 Records_Type;

typedef struct
{
    uint8 tag;
    uint16 data;
} Protocol_Data_Type;

typedef Protocol_Data_Type Protocol_Array_Type[];

enum ProtocolDataType {
    Tag_Phys_Prot_Id = 'P', /* tag for Physical protocol ID */
    Tag_Link_Prot_Id = 'L', /* tag for Link protocol ID */
    Tag_Appl_Prot_Id = 'A', /* tag for Application protocol ID */
    Tag_Data_Type_Id = 'D' /* tag for Data Type ID */
};

typedef uint16 Command_Id_Type;

enum DeviceCommandProtocol1 {
    Cmnd_A010_Abort_Transfer = 0, /* abort current transfer */
    Cmnd_A010_Transfer_Alm = 1, /* transfer almanac */
    Cmnd_A010_Transfer_Posn = 2, /* transfer position */
    Cmnd_A010_Transfer_Prx = 3, /* transfer proximity waypoints */
    Cmnd_A010_Transfer_Rte = 4, /* transfer routes */
    Cmnd_A010_Transfer_Time = 5, /* transfer time */
    Cmnd_A010_Transfer_Trk = 6, /* transfer track log */
    Cmnd_A010_Transfer_Wpt = 7, /* transfer waypoints */
    Cmnd_A010_Turn_Off_Pwr = 8, /* turn off power */
    Cmnd_A010_Start_Pvt_Data = 49, /* start transmitting PVT data */
    Cmnd_A010_Stop_Pvt_Data = 50, /* stop transmitting PVT data */
    Cmnd_A010_FlightBook_Transfer = 92, /* transfer flight records */
    Cmnd_A010_Transfer_Laps = 117, /* transfer fitness laps */
    Cmnd_A010_Transfer_Wpt_Cats = 121, /* transfer waypoint categories */
    Cmnd_A010_Transfer_Runs = 450, /* transfer fitness runs */
    Cmnd_A010_Transfer_Workouts = 451, /* transfer workouts */
    Cmnd_A010_Transfer_Workout_Occurrences = 452, /* transfer workout occurrences */
    Cmnd_A010_Transfer_Fitness_User_Profile = 453, /* transfer fitness user profile */
    Cmnd_A010_Transfer_Workout_Limits = 454, /* transfer workout limits */
    Cmnd_A010_Transfer_Courses = 561, /* transfer fitness courses */
    Cmnd_A010_Transfer_Course_Laps = 562, /* transfer fitness course laps */
    Cmnd_A010_Transfer_Course_Points = 563, /* transfer fitness course points */
    Cmnd_A010_Transfer_Course_Tracks = 564, /* transfer fitness course tracks */
    Cmnd_A010_Transfer_Course_Limits = 565 /* transfer fitness course limits */
};

enum DeviceCommandProtocol2 {
    Cmnd_A011_Abort_Transfer = 0, /* abort current transfer */
    Cmnd_A011_Transfer_Alm = 4, /* transfer almanac */
    Cmnd_A011_Transfer_Rte = 8, /* transfer routes */
    Cmnd_A011_Transfer_Prx = 17, /* transfer proximity waypoints */
    Cmnd_A011_Transfer_Time = 20, /* transfer time */
    Cmnd_A011_Transfer_Wpt = 21, /* transfer waypoints */
    Cmnd_A011_Turn_Off_Pwr = 26 /* turn off power */
};

enum MarineSymbols {
    /*---------------------------------------------------------------
      Marine symbols
      ---------------------------------------------------------------*/
    sym_anchor = 0, /* white anchor symbol */
    sym_bell = 1, /* white bell symbol */
    sym_diamond_grn = 2, /* green diamond symbol */
    sym_diamond_red = 3, /* red diamond symbol */
    sym_dive1 = 4, /* diver down flag 1 */
    sym_dive2 = 5, /* diver down flag 2 */
    sym_dollar = 6, /* white dollar symbol */
    sym_fish = 7, /* white fish symbol */
    sym_fuel = 8, /* white fuel symbol */
    sym_horn = 9, /* white horn symbol */
    sym_house = 10, /* white house symbol */
    sym_knife = 11, /* white knife & fork symbol */
    sym_light = 12, /* white light symbol */
    sym_mug = 13, /* white mug symbol */
    sym_skull = 14, /* white skull and crossbones symbol*/
    sym_square_grn = 15, /* green square symbol */
    sym_square_red = 16, /* red square symbol */
    sym_wbuoy = 17, /* white buoy waypoint symbol */
    sym_wpt_dot = 18, /* waypoint dot */
    sym_wreck = 19, /* white wreck symbol */
    sym_null = 20, /* null symbol (transparent) */
    sym_mob = 21, /* man overboard symbol */
    sym_buoy_ambr = 22, /* amber map buoy symbol */
    sym_buoy_blck = 23, /* black map buoy symbol */
    sym_buoy_blue = 24, /* blue map buoy symbol */
    sym_buoy_grn = 25, /* green map buoy symbol */
    sym_buoy_grn_red = 26, /* green/red map buoy symbol */
    sym_buoy_grn_wht = 27, /* green/white map buoy symbol */
    sym_buoy_orng = 28, /* orange map buoy symbol */
    sym_buoy_red = 29, /* red map buoy symbol */
    sym_buoy_red_grn = 30, /* red/green map buoy symbol */
    sym_buoy_red_wht = 31, /* red/white map buoy symbol */
    sym_buoy_violet = 32, /* violet map buoy symbol */
    sym_buoy_wht = 33, /* white map buoy symbol */
    sym_buoy_wht_grn = 34, /* white/green map buoy symbol */
    sym_buoy_wht_red = 35, /* white/red map buoy symbol */
    sym_dot = 36, /* white dot symbol */
    sym_rbcn = 37, /* radio beacon symbol */
    sym_boat_ramp = 150, /* boat ramp symbol */
    sym_camp = 151, /* campground symbol */
    sym_restrooms = 152, /* restrooms symbol */
    sym_showers = 153, /* shower symbol */
    sym_drinking_wtr = 154, /* drinking water symbol */
    sym_phone = 155, /* telephone symbol */
    sym_1st_aid = 156, /* first aid symbol */
    sym_info = 157, /* information symbol */
    sym_parking = 158, /* parking symbol */
    sym_park = 159, /* park symbol */
    sym_picnic = 160, /* picnic symbol */
    sym_scenic = 161, /* scenic area symbol */
    sym_skiing = 162, /* skiing symbol */
    sym_swimming = 163, /* swimming symbol */
    sym_dam = 164, /* dam symbol */
    sym_controlled = 165, /* controlled area symbol */
    sym_danger = 166, /* danger symbol */
    sym_restricted = 167, /* restricted area symbol */
    sym_null_2 = 168, /* null symbol */
    sym_ball = 169, /* ball symbol */
    sym_car = 170, /* car symbol */
    sym_deer = 171, /* deer symbol */
    sym_shpng_cart = 172, /* shopping cart symbol */
    sym_lodging = 173, /* lodging symbol */
    sym_mine = 174, /* mine symbol */
    sym_trail_head = 175, /* trail head symbol */
    sym_truck_stop = 176, /* truck stop symbol */
    sym_user_exit = 177, /* user exit symbol */
    sym_flag = 178, /* flag symbol */
    sym_circle_x = 179, /* circle with x in the center */
    sym_open_24hr = 180, /* open 24 hours symbol */
    sym_fhs_facility = 181, /* U Fishing Hot Spots Facility */
    sym_bot_cond = 182, /* Bottom Conditions */
    sym_tide_pred_stn = 183, /* Tide/Current Prediction Station */
    sym_anchor_prohib = 184, /* U anchor prohibited symbol */
    sym_beacon = 185, /* U beacon symbol */
    sym_coast_guard = 186, /* U coast guard symbol */
    sym_reef = 187, /* U reef symbol */
    sym_weedbed = 188, /* U weedbed symbol */
    sym_dropoff = 189, /* U dropoff symbol */
    sym_dock = 190, /* U dock symbol */
    sym_marina = 191, /* U marina symbol */
    sym_bait_tackle = 192, /* U bait and tackle symbol */
    sym_stump = 193, /* U stump symbol */
    /*---------------------------------------------------------------
      User customizable symbols
      The values from sym_begin_custom to sym_end_custom inclusive are
      reserved for the identification of user customizable symbols.
      ---------------------------------------------------------------*/
    sym_begin_custom = 7680, /* first user customizable symbol */
    sym_end_custom = 8191, /* last user customizable symbol */
    /*---------------------------------------------------------------
      Land symbols
      ---------------------------------------------------------------*/
    sym_is_hwy = 8192, /* interstate hwy symbol */
    sym_us_hwy = 8193, /* us hwy symbol */
    sym_st_hwy = 8194, /* state hwy symbol */
    sym_mi_mrkr = 8195, /* mile marker symbol */
    sym_trcbck = 8196, /* TracBack (feet) symbol */
    sym_golf = 8197, /* golf symbol */
    sym_sml_cty = 8198, /* small city symbol */
    sym_med_cty = 8199, /* medium city symbol */
    sym_lrg_cty = 8200, /* large city symbol */
    sym_freeway = 8201, /* intl freeway hwy symbol */
    sym_ntl_hwy = 8202, /* intl national hwy symbol */
    sym_cap_cty = 8203, /* capitol city symbol (star) */
    sym_amuse_pk = 8204, /* amusement park symbol */
    sym_bowling = 8205, /* bowling symbol */
    sym_car_rental = 8206, /* car rental symbol */
    sym_car_repair = 8207, /* car repair symbol */
    sym_fastfood = 8208, /* fast food symbol */
    sym_fitness = 8209, /* fitness symbol */
    sym_movie = 8210, /* movie symbol */
    sym_museum = 8211, /* museum symbol */
    sym_pharmacy = 8212, /* pharmacy symbol */
    sym_pizza = 8213, /* pizza symbol */
    sym_post_ofc = 8214, /* post office symbol */
    sym_rv_park = 8215, /* RV park symbol */
    sym_school = 8216, /* school symbol */
    sym_stadium = 8217, /* stadium symbol */
    sym_store = 8218, /* dept. store symbol */
    sym_zoo = 8219, /* zoo symbol */
    sym_gas_plus = 8220, /* convenience store symbol */
    sym_faces = 8221, /* live theater symbol */
    sym_ramp_int = 8222, /* ramp intersection symbol */
    sym_st_int = 8223, /* street intersection symbol */
    sym_weigh_sttn = 8226, /* inspection/weigh station symbol */
    sym_toll_booth = 8227, /* toll booth symbol */
    sym_elev_pt = 8228, /* elevation point symbol */
    sym_ex_no_srvc = 8229, /* exit without services symbol */
    sym_geo_place_mm = 8230, /* Geographic place name, man-made */
    sym_geo_place_wtr = 8231, /* Geographic place name, water */
    sym_geo_place_lnd = 8232, /* Geographic place name, land */
    sym_bridge = 8233, /* bridge symbol */
    sym_building = 8234, /* building symbol */
    sym_cemetery = 8235, /* cemetery symbol */
    sym_church = 8236, /* church symbol */
    sym_civil = 8237, /* civil location symbol */
    sym_crossing = 8238, /* crossing symbol */
    sym_hist_town = 8239, /* historical town symbol */
    sym_levee = 8240, /* levee symbol */
    sym_military = 8241, /* military location symbol */
    sym_oil_field = 8242, /* oil field symbol */
    sym_tunnel = 8243, /* tunnel symbol */
    sym_beach = 8244, /* beach symbol */
    sym_forest = 8245, /* forest symbol */
    sym_summit = 8246, /* summit symbol */
    sym_lrg_ramp_int = 8247, /* large ramp intersection symbol */
    sym_lrg_ex_no_srvc = 8248, /* large exit without services smbl */
    sym_badge = 8249, /* police/official badge symbol */
    sym_cards = 8250, /* gambling/casino symbol */
    sym_snowski = 8251, /* snow skiing symbol */
    sym_iceskate = 8252, /* ice skating symbol */
    sym_wrecker = 8253, /* tow truck (wrecker) symbol */
    sym_border = 8254, /* border crossing (port of entry) */
    sym_geocache = 8255, /* geocache location */
    sym_geocache_fnd = 8256, /* found geocache */
    sym_cntct_smiley = 8257, /* Rino contact symbol, "smiley" */
    sym_cntct_ball_cap = 8258, /* Rino contact symbol, "ball cap" */
    sym_cntct_big_ears = 8259, /* Rino contact symbol, "big ear" */
    sym_cntct_spike = 8260, /* Rino contact symbol, "spike" */
    sym_cntct_goatee = 8261, /* Rino contact symbol, "goatee" */
    sym_cntct_afro = 8262, /* Rino contact symbol, "afro" */
    sym_cntct_dreads = 8263, /* Rino contact symbol, "dreads" */
    sym_cntct_female1 = 8264, /* Rino contact symbol, "female 1" */
    sym_cntct_female2 = 8265, /* Rino contact symbol, "female 2" */
    sym_cntct_female3 = 8266, /* Rino contact symbol, "female 3" */
    sym_cntct_ranger = 8267, /* Rino contact symbol, "ranger" */
    sym_cntct_kung_fu = 8268, /* Rino contact symbol, "kung fu" */
    sym_cntct_sumo = 8269, /* Rino contact symbol, "sumo" */
    sym_cntct_pirate = 8270, /* Rino contact symbol, "pirate" */
    sym_cntct_biker = 8271, /* Rino contact symbol, "biker" */
    sym_cntct_alien = 8272, /* Rino contact symbol, "alien" */
    sym_cntct_bug = 8273, /* Rino contact symbol, "bug" */
    sym_cntct_cat = 8274, /* Rino contact symbol, "cat" */
    sym_cntct_dog = 8275, /* Rino contact symbol, "dog" */
    sym_cntct_pig = 8276, /* Rino contact symbol, "pig" */
    sym_hydrant = 8282, /* water hydrant symbol */
    sym_flag_blue = 8284, /* blue flag symbol */
    sym_flag_green = 8285, /* green flag symbol */
    sym_flag_red = 8286, /* red flag symbol */
    sym_pin_blue = 8287, /* blue pin symbol */
    sym_pin_green = 8288, /* green pin symbol */
    sym_pin_red = 8289, /* red pin symbol */
    sym_block_blue = 8290, /* blue block symbol */
    sym_block_green = 8291, /* green block symbol */
    sym_block_red = 8292, /* red block symbol */
    sym_bike_trail = 8293, /* bike trail symbol */
    sym_circle_red = 8294, /* red circle symbol */
    sym_circle_green = 8295, /* green circle symbol */
    sym_circle_blue = 8296, /* blue circle symbol */
    sym_diamond_blue = 8299, /* blue diamond symbol */
    sym_oval_red = 8300, /* red oval symbol */
    sym_oval_green = 8301, /* green oval symbol */
    sym_oval_blue = 8302, /* blue oval symbol */
    sym_rect_red = 8303, /* red rectangle symbol */
    sym_rect_green = 8304, /* green rectangle symbol */
    sym_rect_blue = 8305, /* blue rectangle symbol */
    sym_square_blue = 8308, /* blue square symbol */
    sym_letter_a_red = 8309, /* red letter 'A' symbol */
    sym_letter_b_red = 8310, /* red letter 'B' symbol */
    sym_letter_c_red = 8311, /* red letter 'C' symbol */
    sym_letter_d_red = 8312, /* red letter 'D' symbol */
    sym_letter_a_green = 8313, /* green letter 'A' symbol */
    sym_letter_c_green = 8314, /* green letter 'C' symbol */
    sym_letter_b_green = 8315, /* green letter 'B' symbol */
    sym_letter_d_green = 8316, /* green letter 'D' symbol */
    sym_letter_a_blue = 8317, /* blue letter 'A' symbol */
    sym_letter_b_blue = 8318, /* blue letter 'B' symbol */
    sym_letter_c_blue = 8319, /* blue letter 'C' symbol */
    sym_letter_d_blue = 8320, /* blue letter 'D' symbol */
    sym_number_0_red = 8321, /* red number '0' symbol */
    sym_number_1_red = 8322, /* red number '1' symbol */
    sym_number_2_red = 8323, /* red number '2' symbol */
    sym_number_3_red = 8324, /* red number '3' symbol */
    sym_number_4_red = 8325, /* red number '4' symbol */
    sym_number_5_red = 8326, /* red number '5' symbol */
    sym_number_6_red = 8327, /* red number '6' symbol */
    sym_number_7_red = 8328, /* red number '7' symbol */
    sym_number_8_red = 8329, /* red number '8' symbol */
    sym_number_9_red = 8330, /* red number '9' symbol */
    sym_number_0_green = 8331, /* green number '0' symbol */
    sym_number_1_green = 8332, /* green number '1' symbol */
    sym_number_2_green = 8333, /* green number '2' symbol */
    sym_number_3_green = 8334, /* green number '3' symbol */
    sym_number_4_green = 8335, /* green number '4' symbol */
    sym_number_5_green = 8336, /* green number '5' symbol */
    sym_number_6_green = 8337, /* green number '6' symbol */
    sym_number_7_green = 8338, /* green number '7' symbol */
    sym_number_8_green = 8339, /* green number '8' symbol */
    sym_number_9_green = 8340, /* green number '9' symbol */
    sym_number_0_blue = 8341, /* blue number '0' symbol */
    sym_number_1_blue = 8342, /* blue number '1' symbol */
    sym_number_2_blue = 8343, /* blue number '2' symbol */
    sym_number_3_blue = 8344, /* blue number '3' symbol */
    sym_number_4_blue = 8345, /* blue number '4' symbol */
    sym_number_5_blue = 8346, /* blue number '5' symbol */
    sym_number_6_blue = 8347, /* blue number '6' symbol */
    sym_number_7_blue = 8348, /* blue number '7' symbol */
    sym_number_8_blue = 8349, /* blue number '8' symbol */
    sym_number_9_blue = 8350, /* blue number '9' symbol */
    sym_triangle_blue = 8351, /* blue triangle symbol */
    sym_triangle_green = 8352, /* green triangle symbol */
    sym_triangle_red = 8353, /* red triangle symbol */
    sym_food_asian = 8359, /* asian food symbol */
    sym_food_deli = 8360, /* deli symbol */
    sym_food_italian = 8361, /* italian food symbol */
    sym_food_seafood = 8362, /* seafood symbol */
    sym_food_steak = 8363, /* steak symbol */
    /*---------------------------------------------------------------
      Aviation symbols
      ---------------------------------------------------------------*/
    sym_airport = 16384, /* airport symbol */
    sym_int = 16385, /* intersection symbol */
    sym_ndb = 16386, /* non-directional beacon symbol */
    sym_vor = 16387, /* VHF omni-range symbol */
    sym_heliport = 16388, /* heliport symbol */
    sym_private = 16389, /* private field symbol */
    sym_soft_fld = 16390, /* soft field symbol */
    sym_tall_tower = 16391, /* tall tower symbol */
    sym_short_tower = 16392, /* short tower symbol */
    sym_glider = 16393, /* glider symbol */
    sym_ultralight = 16394, /* ultralight symbol */
    sym_parachute = 16395, /* parachute symbol */
    sym_vortac = 16396, /* VOR/TACAN symbol */
    sym_vordme = 16397, /* VOR-DME symbol */
    sym_faf = 16398, /* first approach fix */
    sym_lom = 16399, /* localizer outer marker */
    sym_map = 16400, /* missed approach point */
    sym_tacan = 16401, /* TACAN symbol */
    sym_seaplane = 16402, /* Seaplane Base */
};

enum PositionFixType {
    unusable = 0, /* failed integrity check */
    invalid = 1, /* invalid or unavailable */
    DIM_2D = 2, /* two dimensional */
    DIM_3D = 3, /* three dimensional */
    DIM_2D_diff = 4, /* two dimensional differential */
    DIM_3D_diff = 5 /* three dimensional differential */
};

typedef struct {
    sint32 lat; /* latitude in semicircles */
    sint32 lon; /* longitude in semicircles */
} position_type;

typedef struct {
    float64 lat; /* latitude in radians */
    float64 lon; /* longitude in radians */
} radian_position_type;

typedef uint16 symbol_type;

typedef uint32 time_type;

typedef struct {
    char ident[6]; /* identifier */
    position_type posn; /* position */
    unsigned long unused; /* should be set to zero */
    char cmnt[40]; /* comment */
} D100_Wpt_Type;

typedef struct {
    char ident[6]; /* identifier */
    position_type posn; /* position */
    unsigned long unused; /* should be set to zero */
    char cmnt[40]; /* comment */
    float dst; /* proximity distance (meters) */
    unsigned char smbl; /* symbol id */
} D101_Wpt_Type;

typedef struct {
    char ident[6]; /* identifier */
    position_type posn; /* position */
    unsigned long unused; /* should be set to zero */
    char cmnt[40]; /* comment */
    float dst; /* proximity distance (meters) */
    symbol_type smbl; /* symbol id */
} D102_Wpt_Type;

enum D103_Symbol {
    smbl_dot = 0, /* dot symbol */
    smbl_house = 1, /* house symbol */
    smbl_gas = 2, /* gas symbol */
    smbl_car = 3, /* car symbol */
    smbl_fish = 4, /* fish symbol */
    smbl_boat = 5, /* boat symbol */
    smbl_anchor = 6, /* anchor symbol */
    smbl_wreck = 7, /* wreck symbol */
    smbl_exit = 8, /* exit symbol */
    smbl_skull = 9, /* skull symbol */
    smbl_flag = 10, /* flag symbol */
    smbl_camp = 11, /* camp symbol */
    smbl_circle_x = 12, /* circle with x symbol */
    smbl_deer = 13, /* deer symbol */
    smbl_1st_aid = 14, /* first aid symbol */
    smbl_back_track = 15 /* back track symbol */
};

enum D103_Display {
    dspl_name = 0, /* Display symbol with waypoint name */
    dspl_none = 1, /* Display symbol by itself */
    dspl_cmnt = 2 /* Display symbol with comment */
};

typedef struct
{
    char ident[6]; /* identifier */
    position_type posn; /* position */
    unsigned long unused; /* should be set to zero */
    char cmnt[40]; /* comment */
    unsigned char smbl; /* symbol id */
    unsigned char dspl; /* display option */
} D103_Wpt_Type;

enum D104_DisplaySymbol {
    D104_dspl_smbl_none = 0, /* Display symbol by itself */
    D104_dspl_smbl_only = 1, /* Display symbol by itself */
    D104_dspl_smbl_name = 3, /* Display symbol with waypoint name */
    D104_dspl_smbl_cmnt = 5, /* Display symbol with comment */
};

typedef struct {
    char ident[6]; /* identifier */
    position_type posn; /* position */
    uint32 unused; /* should be set to zero */
    char cmnt[40]; /* comment */
    float32 dst; /* proximity distance (meters) */
    symbol_type smbl; /* symbol id */
    uint8 dspl; /* display option */
} D104_Wpt_Type;

typedef struct {
    position_type posn; /* position */
    symbol_type smbl; /* symbol id */
    /* char wpt_ident[]; null-terminated string */
} D105_Wpt_Type;

typedef struct {
    uint8 wpt_class; /* class */
    uint8 subclass[13]; /* subclass */
    position_type posn; /* position */
    symbol_type smbl; /* symbol id */
    /* char wpt_ident[]; null-terminated string */
    /* char lnk_ident[]; null-terminated string */
} D106_Wpt_Type;

enum D107_Color {
    clr_default = 0, /* Default waypoint color */
    clr_red = 1, /* Red */
    clr_green = 2, /* Green */
    clr_blue = 3 /* Blue */
};

typedef struct {
    char ident[6]; /* identifier */
    position_type posn; /* position */
    uint32 unused; /* should be set to zero */
    char cmnt[40]; /* comment */
    uint8 smbl; /* symbol id */
    uint8 dspl; /* display option */
    float32 dst; /* proximity distance (meters) */
    uint8 color; /* waypoint color */
} D107_Wpt_Type;

enum D108_Class {
    D108_user_wpt = 0x00, /* user waypoint */
    D108_avtn_apt_wpt = 0x40, /* aviation airport waypoint */
    D108_avtn_int_wpt = 0x41, /* aviation intersection waypoint */
    D108_avtn_ndb_wpt = 0x42, /* aviation NDB waypoint */
    D108_avtn_vor_wpt = 0x43, /* aviation VOR waypoint */
    D108_avtn_arwy_wpt = 0x44, /* aviation airport runway waypoint */
    D108_avtn_aint_wpt = 0x45, /* aviation airport intersection */
    D108_avtn_andb_wpt = 0x46, /* aviation airport ndb waypoint */
    D108_map_pnt_wpt = 0x80, /* map point waypoint */
    D108_map_area_wpt = 0x81, /* map area waypoint */
    D108_map_int_wpt = 0x82, /* map intersection waypoint */
    D108_map_adrs_wpt = 0x83, /* map address waypoint */
    D108_map_line_wpt = 0x84, /* map line waypoint */
};

enum D108_Color {
    D108_clr_black = 0,
    D108_clr_dark_red = 1,
    D108_clr_dark_green = 2,
    D108_clr_dark_yellow = 3,
    D108_clr_dark_blue = 4,
    D108_clr_dark_magenta = 5,
    D108_clr_dark_cyan = 6,
    D108_clr_light_gray = 7,
    D108_clr_dark_gray = 8,
    D108_clr_red = 9,
    D108_clr_green = 10,
    D108_clr_yellow = 11,
    D108_clr_blue = 12,
    D108_clr_magenta = 13,
    D108_clr_cyan = 14,
    D108_clr_white = 15,
    D108_clr_default_color = 255
};

typedef struct {
    uint8 wpt_class; /* class (see below) */
    uint8 color; /* color (see below) */
    uint8 dspl; /* display options (see below) */
    uint8 attr; /* attributes (see below) */
    symbol_type smbl; /* waypoint symbol */
    uint8 subclass[18]; /* subclass */
    position_type posn; /* position */
    float32 alt; /* altitude in meters */
    float32 dpth; /* depth in meters */
    float32 dist; /* proximity distance in meters */
    char state[2]; /* state */
    char cc[2]; /* country code */
    /* char ident[]; variable length string */
    /* char comment[]; waypoint user comment */
    /* char facility[]; facility name */
    /* char city[]; city name */
    /* char addr[]; address number */
    /* char cross_road[]; intersecting road label */
} D108_Wpt_Type;

typedef struct {
    uint8 dtyp; /* data packet type (0x01 for D109) */
    uint8 wpt_class; /* class */
    uint8 dspl_color; /* display & color (see below) */
    uint8 attr; /* attributes (0x70 for D109) */
    symbol_type smbl; /* waypoint symbol */
    uint8 subclass[18]; /* subclass */
    position_type posn; /* position */
    float32 alt; /* altitude in meters */
    float32 dpth; /* depth in meters */
    float32 dist; /* proximity distance in meters */
    char state[2]; /* state */
    char cc[2]; /* country code */
    uint32 ete; /* outbound link ete in seconds */
    /* char ident[]; variable length string */
    /* char comment[]; waypoint user comment */
    /* char facility[]; facility name */
    /* char city[]; city name */
    /* char addr[]; address number */
    /* char cross_road[]; intersecting road label */
} D109_Wpt_Type;

enum D110_Class {
    D110_user_wpt = 0x00, /* user waypoint */
    D110_avtn_apt_wpt = 0x40, /* aviation airport waypoint */
    D110_avtn_int_wpt = 0x41, /* aviation intersection waypoint */
    D110_avtn_ndb_wpt = 0x42, /* aviation NDB waypoint */
    D110_avtn_vor_wpt = 0x43, /* aviation VOR waypoint */
    D110_avtn_arwy_wpt = 0x44, /* aviation airport runway waypoint */
    D110_avtn_aint_wpt = 0x45, /* aviation airport intersection */
    D110_avtn_andb_wpt = 0x46, /* aviation airport ndb waypoint */
    D110_map_pnt_wpt = 0x80, /* map point waypoint */
    D110_map_area_wpt = 0x81, /* map area waypoint */
    D110_map_int_wpt = 0x82, /* map intersection waypoint */
    D110_map_adrs_wpt = 0x83, /* map address waypoint */
    D110_map_line_wpt = 0x84, /* map line waypoint */
};

enum D110_Color {
    D110_clr_Black = 0,
    D110_clr_Dark_Red = 1,
    D110_clr_Dark_Green = 2,
    D110_clr_Dark_Yellow = 3,
    D110_clr_Dark_Blue = 4,
    D110_clr_Dark_Magenta = 5,
    D110_clr_Dark_Cyan = 6,
    D110_clr_Light_Gray = 7,
    D110_clr_Dark_Gray = 8,
    D110_clr_Red = 9,
    D110_clr_Green = 10,
    D110_clr_Yellow = 11,
    D110_clr_Blue = 12,
    D110_clr_Magenta = 13,
    D110_clr_Cyan = 14,
    D110_clr_White = 15,
    D110_clr_Transparent = 16
};

enum D110_DisplaySymbol {
    D110_dspl_Smbl_Name = 0, /* Display symbol with waypoint name */
    D110_dspl_Smbl_Only = 1, /* Display symbol by itself */
    D110_dspl_Smbl_Comment = 2 /* Display symbol with comment */
};

typedef struct {
    uint8 dtyp; /* data packet type (0x01 for D110) */
    uint8 wpt_class; /* class */
    uint8 dspl_color; /* display & color (see below) */
    uint8 attr; /* attributes (0x80 for D110) */
    symbol_type smbl; /* waypoint symbol */
    uint8 subclass[18]; /* subclass */
    position_type posn; /* position */
    float32 alt; /* altitude in meters */
    float32 dpth; /* depth in meters */
    float32 dist; /* proximity distance in meters */
    char state[2]; /* state */
    char cc[2]; /* country code */
    uint32 ete; /* outbound link ete in seconds */
    float32 temp; /* temperature */
    time_type time; /* timestamp */
    uint16 wpt_cat; /* category membership */
    /* char ident[]; variable length string */
    /* char comment[]; waypoint user comment */
    /* char facility[]; facility name */
    /* char city[]; city name */
    /* char addr[]; address number */
    /* char cross_road[]; intersecting road label */
} D110_Wpt_Type;

typedef struct {
    char name[17]; /* category name */
} D120_Wpt_Cat_Type;

enum D150_Class {
    D150_apt_wpt_class = 0, /* airport waypoint class */
    D150_int_wpt_class = 1, /* intersection waypoint class */
    D150_ndb_wpt_class = 2, /* NDB waypoint class */
    D150_vor_wpt_class = 3, /* VOR waypoint class */
    D150_usr_wpt_class = 4, /* user defined waypoint class */
    D150_rwy_wpt_class = 5, /* airport runway threshold waypoint class */
    D150_aint_wpt_class = 6, /* airport intersection waypoint class */
    D150_locked_wpt_class = 7 /* locked waypoint class */
};

typedef struct {
    char ident[6]; /* identifier */
    char cc[2]; /* country code */
    uint8 wpt_class; /* class */
    position_type posn; /* position */
    sint16 alt; /* altitude (meters) */
    char city[24]; /* city */
    char state[2]; /* state */
    char name[30]; /* facility name */
    char cmnt[40]; /* comment */
} D150_Wpt_Type;

enum D151_Class {
    D151_apt_wpt_class = 0, /* airport waypoint class */
    D151_vor_wpt_class = 1, /* VOR waypoint class */
    D151_usr_wpt_class = 2, /* user defined waypoint class */
    D151_locked_wpt_class = 3 /* locked waypoint class */
};

typedef struct
{
    char ident[6]; /* identifier */
    position_type posn; /* position */
    uint32 unused; /* should be set to zero */
    char cmnt[40]; /* comment */
    float32 dst; /* proximity distance (meters) */
    char name[30]; /* facility name */
    char city[24]; /* city */
    char state[2]; /* state */
    sint16 alt; /* altitude (meters) */
    char cc[2]; /* country code */
    char unused2; /* should be set to zero */
    uint8 wpt_class; /* class */
} D151_Wpt_Type;

enum D152_Class {
    D152_apt_wpt_class = 0, /* airport waypoint class */
    D152_int_wpt_class = 1, /* intersection waypoint class */
    D152_ndb_wpt_class = 2, /* NDB waypoint class */
    D152_vor_wpt_class = 3, /* VOR waypoint class */
    D152_usr_wpt_class = 4, /* user defined waypoint class */
    D152_locked_wpt_class = 5 /* locked waypoint class */
};

typedef struct {
    char ident[6]; /* identifier */
    position_type posn; /* position */
    uint32 unused; /* should be set to zero */
    char cmnt[40]; /* comment */
    float32 dst; /* proximity distance (meters) */
    char name[30]; /* facility name */
    char city[24]; /* city */
    char state[2]; /* state */
    sint16 alt; /* altitude (meters) */
    char cc[2]; /* country code */
    uint8 unused2; /* should be set to zero */
    uint8 wpt_class; /* class */
} D152_Wpt_Type;

enum D154_Class {
    D154_apt_wpt_class = 0, /* airport waypoint class */
    D154_int_wpt_class = 1, /* intersection waypoint class */
    D154_ndb_wpt_class = 2, /* NDB waypoint class */
    D154_vor_wpt_class = 3, /* VOR waypoint class */
    D154_usr_wpt_class = 4, /* user defined waypoint class */
    D154_rwy_wpt_class = 5, /* airport runway threshold waypoint class */
    D154_aint_wpt_class = 6, /* airport intersection waypoint class */
    D154_andb_wpt_class = 7, /* airport NDB waypoint class */
    D154_sym_wpt_class = 8, /* user defined symbol-only waypoint class */
    D154_locked_wpt_class = 9 /* locked waypoint class */
};

typedef struct
{
    char ident[6]; /* identifier */
    position_type posn; /* position */
    uint32 unused; /* should be set to zero */
    char cmnt[40]; /* comment */
    float32 dst; /* proximity distance (meters) */
    char name[30]; /* facility name */
    char city[24]; /* city */
    char state[2]; /* state */
    sint16 alt; /* altitude (meters) */
    char cc[2]; /* country code */
    uint8 unused2; /* should be set to zero */
    uint8 wpt_class; /* class */
    symbol_type smbl; /* symbol id */
} D154_Wpt_Type;

typedef uint8 D200_Rte_Hdr_Type; /* route number */

typedef struct
{
    uint8 nmbr; /* route number */
    char cmnt[20]; /* comment */
} D201_Rte_Hdr_Type;

typedef struct
{
    /* char rte_ident[]; variable length string */
} D202_Rte_Hdr_Type;

enum D155_Class {
    D155_apt_wpt_class = 0, /* airport waypoint class */
    D155_int_wpt_class = 1, /* intersection waypoint class */
    D155_ndb_wpt_class = 2, /* NDB waypoint class */
    D155_vor_wpt_class = 3, /* VOR waypoint class */
    D155_usr_wpt_class = 4, /* user defined waypoint class */
    D155_locked_wpt_class = 5 /* locked waypoint class */
};

enum D155_DisplaySymbol {
    D155_dspl_smbl_only = 1, /* Display symbol by itself */
    D155_dspl_smbl_name = 3, /* Display symbol with waypoint name */
    D155_dspl_smbl_cmnt = 5, /* Display symbol with comment */
};

enum D210_Class {
    line = 0,
    d210_link = 1,
    net = 2,
    direct = 3,
    snap = 0xFF
};

typedef struct {
    uint16 linkClass; /* link class; see below */
    uint8 subclass[18]; /* subclass */
    /* char ident[]; variable length string */
} D210_Rte_Link_Type;

typedef struct {
    char ident[6]; /* identifier */
    position_type posn; /* position */
    uint32 unused; /* should be set to zero */
    char cmnt[40]; /* comment */
    float32 dst; /* proximity distance (meters) */
    char name[30]; /* facility name */
    char city[24]; /* city */
    char state[2]; /* state */
    sint16 alt; /* altitude (meters) */
    char cc[2]; /* country code */
    uint8 unused2; /* should be set to zero */
    uint8 wpt_class; /* class */
    symbol_type smbl; /* symbol id */
    uint8 dspl; /* display option */
} D155_Wpt_Type;

typedef struct {
    position_type posn; /* position */
    time_type time; /* time */
    bool new_trk; /* new track segment? */
} D300_Trk_Point_Type;

typedef struct {
    position_type posn; /* position */
    time_type time; /* time */
    float32 alt; /* altitude in meters */
    float32 dpth; /* depth in meters */
    bool new_trk; /* new track segment? */
} D301_Trk_Point_Type;

typedef struct {
    position_type posn; /* position */
    time_type time; /* time */
    float32 alt; /* altitude in meters */
    float32 dpth; /* depth in meters */
    float32 temp; /* temp in degrees C */
    bool new_trk; /* new track segment? */
} D302_Trk_Point_Type;

typedef struct {
    position_type posn; /* position */
    time_type time; /* time */
    float32 alt; /* altitude in meters */
    uint8 heart_rate; /* heart rate in beats per minute */
} D303_Trk_Point_Type;

typedef struct {
    position_type posn; /* position */
    time_type time; /* time */
    float32 alt; /* altitude in meters */
    float32 distance; /* distance traveled in meters. See below. */
    uint8 heart_rate; /* heart rate in beats per minute */
    uint8 cadence; /* in revolutions per minute */
    bool sensor; /* is a wheel sensor present? */
} D304_Trk_Point_Type;

typedef struct {
    bool dspl; /* display on the map? */
    uint8 color; /* color (same as D108) */
    /* char trk_ident[]; null-terminated string */
} D310_Trk_Hdr_Type;

typedef struct {
    uint16 index; /* unique among all tracks received from device
                   */
} D311_Trk_Hdr_Type;

enum D312_Color {
    D312_clr_Black = 0,
    D312_clr_Dark_Red = 1,
    D312_clr_Dark_Green = 2,
    D312_clr_Dark_Yellow = 3,
    D312_clr_Dark_Blue = 4,
    D312_clr_Dark_Magenta = 5,
    D312_clr_Dark_Cyan = 6,
    D312_clr_Light_Gray = 7,
    D312_clr_Dark_Gray = 8,
    D312_clr_Red = 9,
    D312_clr_Green = 10,
    D312_clr_Yellow = 11,
    D312_clr_Blue = 12,
    D312_clr_Magenta = 13,
    D312_clr_Cyan = 14,
    D312_clr_White = 15,
    D312_clr_Transparent = 16,
    D312_clr_DefaultColor = 255
};

typedef struct {
    bool dspl; /* display on the map? */
    uint8 color; /* color (see below) */
    /* char trk_ident[]; null-terminated string */
} D312_Trk_Hdr_Type;

typedef struct {
    D100_Wpt_Type wpt; /* waypoint */
    float32 dst; /* proximity distance (meters) */
} D400_Prx_Wpt_Type;

typedef struct {
    D103_Wpt_Type wpt; /* waypoint */
    float32 dst; /* proximity distance (meters) */
} D403_Prx_Wpt_Type;

typedef struct {
    int idx; /* proximity index */
    D150_Wpt_Type wpt; /* waypoint */
    float32 dst; /* proximity distance (meters) */
} D450_Prx_Wpt_Type;

typedef struct {
    uint16 wn; /* week number (weeks) */
    float32 toa; /* almanac data reference time (s) */
    float32 af0; /* clock correction coefficient (s) */
    float32 af1; /* clock correction coefficient (s/s) */
    float32 e; /* eccentricity (-) */
    float32 sqrta; /* square root of semi-major axis (a)(m**1/2) */
    float32 m0; /* mean anomaly at reference time (r) */
    float32 w; /* argument of perigee (r) */
    float32 omg0; /* right ascension (r) */
    float32 odot; /* rate of right ascension (r/s) */
    float32 i; /* inclination angle (r) */
} D500_Almanac_Type;

typedef struct {
    uint16 wn; /* week number (weeks) */
    float32 toa; /* almanac data reference time (s) */
    float32 af0; /* clock correction coefficient (s) */
    float32 af1; /* clock correction coefficient (s/s) */
    float32 e; /* eccentricity (-) */
    float32 sqrta; /* square root of semi-major axis (a)(m**1/2) */
    float32 m0; /* mean anomaly at reference time (r) */
    float32 w; /* argument of perigee (r) */
    float32 omg0; /* right ascension (r) */
    float32 odot; /* rate of right ascension (r/s) */
    float32 i; /* inclination angle (r) */
    uint8 hlth; /* almanac health */
} D501_Almanac_Type;

typedef struct {
    uint8 svid; /* satellite id */
    uint16 wn; /* week number (weeks) */
    float32 toa; /* almanac data reference time (s) */
    float32 af0; /* clock correction coefficient (s) */
    float32 af1; /* clock correction coefficient (s/s) */
    float32 e; /* eccentricity (-) */
    float32 sqrta; /* square root of semi-major axis (a)(m**1/2) */
    float32 m0; /* mean anomaly at reference time (r) */
    float32 w; /* argument of perigee (r) */
    float32 omg0; /* right ascension (r) */
    float32 odot; /* rate of right ascension (r/s) */
    float32 i; /* inclination angle (r) */
} D550_Almanac_Type;

typedef struct {
    uint8 svid; /* satellite id */
    uint16 wn; /* week number (weeks) */
    float32 toa; /* almanac data reference time (s) */
    float32 af0; /* clock correction coefficient (s) */
    float32 af1; /* clock correction coefficient (s/s) */
    float32 e; /* eccentricity (-) */
    float32 sqrta; /* square root of semi-major axis (a)(m**1/2) */
    float32 m0; /* mean anomaly at reference time (r) */
    float32 w; /* argument of perigee (r) */
    float32 omg0; /* right ascension (r) */
    float32 odot; /* rate of right ascension (r/s) */
    float32 i; /* inclination angle (r) */
    uint8 hlth; /* almanac health bits 17:24 (coded) */
} D551_Almanac_Type;

typedef struct {
    uint8 month; /* month (1-12) */
    uint8 day; /* day (1-31) */
    uint16 year; /* year (1990 means 1990) */
    uint16 hour; /* hour (0-23) */
    uint8 minute; /* minute (0-59) */
    uint8 second; /* second (0-59) */
} D600_Date_Time_Type;

typedef struct {
    time_type takeoff_time; /* Time flight started */
    time_type landing_time; /* Time flight ended */
    position_type takeoff_posn; /* Takeoff lat/lon */
    position_type landing_posn; /* Takeoff lat/lon */
    uint32 night_time; /* Seconds flown in night time conditions */
    uint32 num_landings; /* Number of landings during the flight */
    float32 max_speed; /* Max velocity during flight (meters/sec) */
    float32 max_alt; /* Max altitude above WGS84 ellipsoid (meters)*/
    float32 distance; /* Distance of flight (meters) */
    bool cross_country_flag; /* Flight met cross country criteria */
    /* char departure_name[]; Name of airport <= 31 bytes */
    /* char departure_ident[]; ID of airport <= 11 bytes */
    /* char arrival_name[]; Name of airport <= 31 bytes */
    /* char arrival_ident[];ID of airport <= 11 bytes */
    /* char ac_id[]; N Number of airplane <= 11 bytes */
} D650_Flight_Book_Record_Type;

typedef radian_position_type D700_Position_Type;

typedef struct {
    float32 alt; /* altitude above WGS 84 ellipsoid (meters) */
    float32 epe; /* estimated position error, 2 sigma (meters) */
    float32 eph; /* epe, but horizontal only (meters) */
    float32 epv; /* epe, but vertical only (meters) */
    uint16 fix; /* type of position fix */
    float64 tow; /* time of week (seconds) */
    radian_position_type posn; /* latitude and longitude (radians) */
    float32 east; /* velocity east (meters/second) */
    float32 north; /* velocity north (meters/second) */
    float32 up; /* velocity up (meters/second) */
    float32 msl_hght; /* height of WGS84 ellipsoid above MSL(meters)*/
    sint16 leap_scnds; /* difference between GPS and UTC (seconds) */
    uint32 wn_days; /* week number days */
} D800_Pvt_Data_Type;

typedef struct {
    time_type start_time;
    uint32 total_time; /* In hundredths of a second */
    float32 total_distance; /* In meters */
    position_type begin; /* Invalid if both lat and lon are 0x7FFFFFFF */
    position_type end; /* Invalid if both lat and lon are 0x7FFFFFFF */
    uint16 calories;
    uint8 track_index; /* See below */
    uint8 unused; /* Unused. Set to 0. */
} D906_Lap_Type;

typedef struct {
    uint32 num_valid_steps; /* Number of valid steps (1-20) */
    struct
    {
        char custom_name[16]; /* Null-terminated step name */
        float32 target_custom_zone_low; /* See below */
        float32 target_custom_zone_high; /* See below */
        uint16 duration_value; /* See below */
        uint8 intensity; /* Same as D1001 */
        uint8 duration_type; /* See below */
        uint8 target_type; /* See below */
        uint8 target_value; /* See below */
        uint16 unused; /* Unused. Set to 0. */
    } steps[20];
    char name[16]; /* Null-terminated workout name */
    uint8 sport_type; /* Same as D1000 */
} D1002_Workout_Type;

enum D1000_ProgramType {
    D1000_none = 0,
    D1000_virtual_partner = 1, /* Completed with Virtual Partner */
    D1000_workout = 2 /* Completed as part of a workout */
};

enum D1000_SportType {
    running = 0,
    biking = 1,
    other = 2
};

typedef struct {
    uint32 track_index; /* Index of associated track */
    uint32 first_lap_index; /* Index of first associated lap */
    uint32 last_lap_index; /* Index of last associated lap */
    uint8 sport_type; /* See below */
    uint8 program_type; /* See below */
    uint16 unused; /* Unused. Set to 0. */
    struct
    {
        uint32 time; /* Time result of virtual partner */
        float32 distance; /* Distance result of virtual partner */
    } virtual_partner;
    D1002_Workout_Type workout; /* Workout */
} D1000_Run_Type;

typedef struct {
    uint32 index; /* Unique among all laps received from device */
    time_type start_time; /* Start of lap time */
    uint32 total_time; /* Duration of lap, in hundredths of a second */
    float32 total_dist; /* Distance in meters */
    float32 max_speed; /* In meters per second */
    position_type begin; /* Invalid if both lat and lon are 0x7FFFFFFF */
    position_type end; /* Invalid if both lat and lon are 0x7FFFFFFF */
    uint16 calories; /* Calories burned this lap */
    uint8 avg_heart_rate; /* In beats-per-minute, 0 if invalid */
    uint8 max_heart_rate; /* In beats-per-minute, 0 if invalid */
    uint8 intensity; /* See below */
} D1001_Lap_Type;

enum {
    active = 0, /* This is a standard, active lap */
    rest = 1 /* This is a rest lap in a workout */
};

typedef struct {
    char workout_name[16]; /* Null-terminated workout name */
    time_type day; /* Day on which the workout falls */
} D1003_Workout_Occurrence_Type;

typedef struct {
    struct {
        struct {
            uint8 low_heart_rate; /* In beats-per-minute, must be > 0 */
            uint8 high_heart_rate; /* In beats-per-minute, must be > 0 */
            uint16 unused; /* Unused. Set to 0. */
        } heart_rate_zones[5];
        struct {
            float32 low_speed; /* In meters-per-second */
            float32 high_speed; /* In meters-per-second */
            char name[16]; /* Null-terminated speed-zone name */
        } speed_zones[10];
        float32 gear_weight; /* Weight of equipment in kilograms */
        uint8 max_heart_rate; /* In beats-per-minute, must be > 0 */
        uint8 unused1; /* Unused. Set to 0. */
        uint16 unused2; /* Unused. Set to 0. */
    } activities[3];
    float32 weight; /* User's weight, in kilograms */
    uint16 birth_year; /* No base value (i.e. 1990 means 1990) */
    uint8 birth_month; /* 1 = January, etc. */
    uint8 birth_day; /* 1 = first day of month, etc. */
    uint8 gender; /* See below */
} D1004_Fitness_User_Profile_Type;

enum {
    female = 0,
    male = 1
};

typedef struct {
    uint32 max_workouts; /* Maximum workouts */
    uint32 max_unscheduled_workouts; /* Maximum unscheduled workouts */
    uint32 max_occurrences; /* Maximum workout occurrences */
} D1005_Workout_Limits;

typedef struct {
    uint16 index; /* Unique among courses on device */
    uint16 unused; /* Unused. Set to 0. */
    char course_name[16]; /* Null-terminated, unique course name */
    uint16 track_index; /* Index of the associated track */
} D1006_Course_Type;

typedef struct {
    uint16 course_index; /* Index of associated course */
    uint16 lap_index; /* This lap's index in the course */
    uint32 total_time; /* In hundredths of a second */
    float32 total_dist; /* In meters */
    position_type begin; /* Starting position of the lap */
    position_type end; /* Final position of the lap */
    uint8 avg_heart_rate; /* In beats-per-minute */
    uint8 max_heart_rate; /* In beats-per-minute */
    uint8 intensity; /* Same as D1001 */
    uint8 avg_cadence; /* In revolutions-per-minute */
} D1007_Course_Lap_Type;

typedef struct {
    uint32 num_valid_steps; /* Number of valid steps (1-20) */
    struct
    {
        char custom_name[16]; /* Null-terminated step name */
        float32 target_custom_zone_low; /* See below */
        float32 target_custom_zone_high; /* See below */
        uint16 duration_value; /* Same as D1002 */
        uint8 intensity; /* Same as D1001 */
        uint8 duration_type; /* Same as D1002 */
        uint8 target_type; /* See below */
        uint8 target_value; /* See below */
        uint16 unused; /* Unused. Set to 0. */
    } steps[20];
    char name[16]; /* Null-terminated workout name */
    uint8 sport_type; /* Same as D1000 */
} D1008_Workout_Type;

typedef struct {
    uint16 track_index; /* Index of associated track */
    uint16 first_lap_index; /* Index of first associated lap */
    uint16 last_lap_index; /* Index of last associated lap */
    uint8 sport_type; /* Same as D1000 */
    uint8 program_type; /* See below */
    uint8 multisport; /* See below */
    uint8 unused1; /* Unused. Set to 0. */
    uint16 unused2; /* Unused. Set to 0. */
    struct
    {
        uint32 time; /* Time result of quick workout */
        float32 distance; /* Distance result of quick workout */
    } quick_workout;
    D1008_Workout_Type workout; /* Workout */
} D1009_Run_Type;

enum {
    no = 0, /* Not a MultiSport run */
    yes = 1, /* Part of a MultiSport session */
    yesAndLastInGroup = 2 /* The last of a MultiSport session */
};

enum D1010_ProgramType {
    D1010_none = 0,
    D1010_virtual_partner = 1, /* Completed with Virtual Partner */
    D1010_workout = 2, /* Completed as part of a workout */
    D1010_auto_multisport = 3 /* Completed as part of an auto MultiSport */
};

typedef struct {
    uint32 track_index; /* Index of associated track */
    uint32 first_lap_index; /* Index of first associated lap */
    uint32 last_lap_index; /* Index of last associated lap */
    uint8 sport_type; /* Sport type (same as D1000) */
    uint8 program_type; /* See below */
    uint8 multisport; /* Same as D1009 */
    uint8 unused; /* Unused. Set to 0. */
    struct
    {
        uint32 time; /* Time result of virtual partner */
        float32 distance; /* Distance result of virtual partner */
    } virtual_partner;
    D1002_Workout_Type workout; /* Workout */
} D1010_Run_Type;

enum D1011_Trigger_Method {
    D1011_manual = 0,
    D1011_distance = 1,
    D1011_location = 2,
    D1011_time = 3,
    D1011_heart_rate = 4
};

typedef struct {
    uint16 index; /* Unique among all laps received from device */
    uint16 unused; /* Unused. Set to 0. */
    time_type start_time; /* Start of lap time */
    uint32 total_time; /* Duration of lap, in hundredths of a second */
    float32 total_dist; /* Distance in meters */
    float32 max_speed; /* In meters per second */
    position_type begin; /* Invalid if both lat and lon are 0x7FFFFFFF */
    position_type end; /* Invalid if both lat and lon are 0x7FFFFFFF */
    uint16 calories; /* Calories burned this lap */
    uint8 avg_heart_rate; /* In beats-per-minute, 0 if invalid */
    uint8 max_heart_rate; /* In beats-per-minute, 0 if invalid */
    uint8 intensity; /* Same as D1001 */
    uint8 avg_cadence; /* In revolutions-per-minute, 0xFF if invalid */
    uint8 trigger_method; /* See below */
} D1011_Lap_Type;

typedef struct {
    char name[11]; /* Null-terminated name */
    uint8 unused1; /* Unused. Set to 0. */
    uint16 course_index; /* Index of associated course */
    uint16 unused2; /* Unused. Set to 0. */
    time_type track_point_time; /* Time */
    uint8 point_type; /* See below */
} D1012_Course_Point_Type;

enum {
    generic = 0,
    summit = 1,
    valley = 2,
    water = 3,
    food = 4,
    danger = 5,
    left = 6,
    right = 7,
    straight = 8,
    first_aid = 9,
    fourth_category = 10,
    third_category = 11,
    second_category = 12,
    first_category = 13,
    hors_category = 14,
    sprint = 15
};

typedef struct {
    uint32 max_courses; /* Maximum courses */
    uint32 max_course_laps; /* Maximum course laps */
    uint32 max_course_pnt; /* Maximum course points */
    uint32 max_course_trk_pnt; /* Maximum course track points */
} D1013_Course_Limits_Type;

#pragma pack( pop)

class Garmin
{

public:

    Garmin();

    ~Garmin();

    void readInputOutputStream(
                               iostream &inout,
                               bool *stopped);

protected:

    virtual void pvtData(D800_Pvt_Data_Type &pvtData);

private:

    Packet packet;
    char *data;
    unsigned long currentDataSize;

};

#endif
