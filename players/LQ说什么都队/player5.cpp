#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include <queue>
const double PI=3.1415926;

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous =false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::StraightAStudent
};

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::ANoisyPerson;
bool move = false;

/*----------------------本代码简要介绍-------------------------*/
/*
    本代码的主要思路是Student（职业为StraightAStudent）选择一个合适的位置躲起来，等到比赛将要结束时前往作业处进行学习；而Tricker（职业为ANoisyPerson）则是沿着固定的路线前进
    本代码并无任何的高级寻路算法，寻路是通过在每个教室标记一些特殊点（如TB1node数组），定义了在每两个标记点间的最短路径，通过字符串数组来保存（如TB3oath），将整个地图化简；
代码中的许多函数并不为最终的策略服务，原因是在设计时我们游戏的策略仍然为较普通的找教室学习and看到Tricker就躲避。
    我们注意到这个游戏的双方阵营较为不平衡，再强的学生面对拥有一定实力的Tricker都很容易惨败（输几千分，而且学生活的越久越便于Tricker刷分），但是决赛的规则是计算双方交换阵容
两次比赛的得分和。于是我们在作为学生阵营时让学生躲起来，以此应对一些不会搜草的队伍，就算不能取胜净胜分也不会很低，而在作为Tricker阵营时利用游戏的较不平衡性获取更高的净胜分，
从而在比赛中胜出

*/


enum class TrickerStatus
{
    Init,
    Tracing,
    Attaking, 
    Searching,
    GotoGate,
};
enum class StudentStatus
{
    WantToStudy,
    Studying,
    Init,
    WantToGraduate,
    Graduating,
    Running,
    GoToWaitingPoint,
    GoToStudy,
    Studying1,
};
struct MapNode
{
    int x, y;
    bool operator != (const MapNode& n)
    {
        if ((this->x != n.x) || (this->y != n.y))
            return 1;
        else return 0;
    }
    bool operator == (const MapNode& n)
    {
        if ((this->x == n.x) && (this->y == n.y))
            return 1;
        else return 0;
    }
};
struct WindowNode
{
    int x_in, y_in, x_out, y_out;
};
struct Classroom
{
    int id;
    long int progress;
    MapNode xy[4];
    MapNode realxy;
};
struct Gate
{
    int progress;
    MapNode xy;
    int Status;//0代表未刷新，1代表已刷新（正常的2个gate都是1）
};

static THUAI6::PlaceType map[50][50];//保存地图信息
MapNode path[1000];//保存该玩家到达想去的地方的路径
int pathnum = 0;//记录path的下标
int pathlength = 0;//记录path的长度

//保存各个教室的关键节点坐标（待补充）
MapNode TB3node[7] = { {15,6},{23,6},{21,15},{13,16} ,{20,5} ,{24,16},{17,19} };
MapNode TB5node[10] = { { 9,28 }, { 8,36 }, { 8,41 }, { 11,43 }, { 15,37 }, { 16,33 }, { 19,33 },{7,33},{18,39},{17,42} };
MapNode TB6node[11] = { { 39,19 },{ 34,25 },{ 30,25 },{ 28,34 },{ 33,35 },{ 40,27 },{ 46,32 },{43,31},{31,38},{35,42},{27,28} };
MapNode TB1node[29] = { 
    {8,3},{7,10},{11,16},{10,21},{9,24},{14,26},{22,25},{26,22},{29,19},{31,12},
    {37,8},{42,3},{46,12},{46,21},{24,31},{24,38},{28,44},{35,47},{41,42},{45,41},
    {21,43},{21,48},{5,48},{5,42},{2,37},{2,20},{2,10},{11,31},{15,48} };
MapNode TB1node2[16] = {
    {11,11},{1,1},{11,1},{1,11},
    {11,38},{1,48},{1,38},{11,48},
    {38,11},{48,1},{38,1},{48,11},
    {38,38},{48,48},{38,48},{48,38}
};
//各个教室的关键节点个数（待补充）
const int sizeof_TB3node = 7;
const int sizeof_TB5node = 10;
const int sizeof_TB6node = 11;
const int sizeof_TB1node = 29;
const int sizeof_TB1node2 = 16;

//各个教室的节点间最短路径（待补充）
const char* TB3path[7][7] = {
    {"0","01","012","03","014","0125","036"},
    {"10","1","12","103","14","15","126"},
    {"210","21","2","23","214","25","26"},
    {"30","301","32","3","3214","325","36"},
    {"410","41","412","4123","4","45","4126"},
    {"510","51","52","523","54","526"},
    {"630","621","60","63","6214","625"}
};
const char* TB5path[10][10] = {
    {"0","01","012","0123","014","0145","01456","017","0148","0149"},
    {"10","1","12","123","14","145","1456","17","148","149"},
    {"210","21","2","23","234","2345","23456","27","238","2349"},
    {"3210","321","32","3","34","345","3456","327","38","349"},
    {"410","41","412","43","4","45","46","417","48","49"},
    {"5410","541","5412","543","54","5","56","5417","568","549"},
    {"65410","6541","65432","6543","654","65","6","65417","68","6549"},
    {"710","71","72","723","714","7145","71456","7","7148","7149"},
    {"8410","841","832","83","84","865","86","8417","8","89"},
    {"9410","941","9432","943","94","945","9456","9417","98","9"}
};
const char* TB6path[11][11] = {
    {"0","01","012","0123","054","05","056","057","0548","0549","0543:"},
    {"10","1","12","123","154","15","156","157","1548","1549","1543:"},
    {"210","21","2","23","234","215","2156","2157","21548","21549","23:"},
    {"3210","321","32","3","34","345","3456","3457","38","349","3:"},
    {"450","451","432","43","4","45","456","457","48","49","43:"},
    {"50","51","512","543","54","5","56","57","548","549","543:"},
    {"650","651","6512","6543","654","65","6","657","6548","6549","6543:"},
    {"7510","751","7512","7543","754","75","756","7","7548","7549","7543:"},
    {"83210","8321","832","83","84","845","8456","8457","8","849","83:"},
    {"9450","9451","94512","943","94","945","9456","9457","948","943:"},
    {":3250",":3451",":32",":3",":34",":345",":3456","3457",":38",":349",":"}
};
const char* TB1path[29][29] = {
    {"0","01","012","0123","01234","01235","012356","0123567","01235678","012356789","012356789:","012356789:;","012356789:;<","012356789:;<=","012356>","012356>?","012356>?@","012356>?@A","012356>?@AB","012356>?@ABC","012356>?D","012356>?DE","01JIHGF","01JIHG","01JIH","01JI","01J","012K","01JIHGFL"},
    {"10","1","12","123","1234","1235","12356","123567","1235678","12356789","12356789:","12356789:;","12356789:;<","12356789:;<=","12356>","12356>?","12356>?@","12356>?@A","12356>?@AB","12356>?@ABC","12356>?D","12356>?DE","1JIHGF","1JIHG","1JIH","1JI","1J","12K","1JIHGFL"},
    {"210","21","2","23","234","235","2356","23567","235678","2356789","2356789:","2356789:;","2356789:;<","2356789:;<=","2356>","2356>?","2356>?@","2356>?@A","2356>?@AB","2356>?@ABC","2356>?D","2356>?DE","23IHGF","23IHG","23IH","23I","21J","2K","23IHGFL"},
    {"3210","321","32","3","34","35","356","3567","35678","356789","356789:","356789:;","356789:;<","356789:;<=","356>","356>?","356>?@","356>?@A","356>?@AB","356>?@ABC","356>?D","356>?DE","3IHGF","3IHG","3IH","3I","3IJ","35K","23IHGFL"},
    {"43210","4321","432","43","4","45","456","4567","45678","456789","456789:","456789:;","456789:;<","456789:;<=","456>","456>?","456>?@","456>?@A","456>?@AB","456>?@ABC","456>?D","456>?DE","43IHGF","43IHG","43IH","43I","43IJ","45K","43IHGFL"},
    {"53210","5321","532","53","54","5","56","567","5678","56789","56789:","56789:;","56789:;<","56789:;<=","56>","56>?","56>?@","56>?@A","56>?@AB","56>?@ABC","56>?D","56>?DE","53IHGF","53IHG","53IH","53I","53IJ","5K","56>?DEL"},
    {"653210","65321","6532","653","654","65","6","67","678","6789","6789:","6789:;","6789:;<","6789:;<=","6>","6>?","6>?@","6>?@A","6>?@AB","6>?@ABC","6>?D","6>?DE","6>?DEF","6>?DEFG","653IH","653I","653IJ","65K","6>?DEL"},
    {"7653210","765321","76532","7653","7654","765","76","7","78","789","789:","789:;","789:;<","789:;<=","76>","76>?","76>?@","76>?@A","76>?@AB","76>?@ABC","76>?D","76>?DE","76>?DEF","76>?DEFG","7653IH","7653I","7653IJ","765K","76>?DEL"},
    {"87653210","8765321","876532","87653","87654","8765","876","87","8","89","89:","89:;","89:;<","89:;<=","876>","876>?","876>?@","876>?@A","876>?@AB","876>?@ABC","876>?D","876>?DE","876>?DEF","876>?DEFG","87653IH","87653I","87653IJ","8765K","876>?DEL"},
    {"987653210","98765321","9876532","987653","987654","98765","9876","987","98","9","9:","9:;","9:;<","9:;<=","9876>","9876>?","9876>?@","9876>?@A","9876>?@AB","9876>?@ABC","9876>?D","9876>?DE","9876>?DEF","9876>?DEFG","987653IH","987653I","987653IJ","98765K","9876>?DEL"},
    {":987653210",":98765321",":9876532",":987653",":987654",":98765",":9876",":987",":98",":9",":",":;",":;<",":;<=",":9876>",":9876>?",":9876>?@",":9876>?@A",":9876>?@AB",":9876>?@ABC",":9876>?D",":9876>?DE",":9876>?DEF",":9876>?DEFG",":987653IH",":987653I",":987653IJ",":98765K",":9876>?DEL"},
    {";:987653210",";:98765321",";:9876532",";:987653",";:987654",";:98765",";:9876",";:987",";:98",";:9",";:",";",";<",";<=",";:9876>",";:9876>?",";:9876>?@",";:9876>?@A",";:9876>?@AB",";:9876>?@ABC",";:9876>?D",";:9876>?DE",";:9876>?DEF",";:9876>?DEFG",";:987653IH",";:987653I",";:987653IJ",";:98765K",";:9876>?DEL"},
    {"<;:987653210","<;:98765321","<;:9876532","<;:987653","<;:987654","<;:98765","<;:9876","<;:987","<;:98","<;:9","<;:","<;","<","<=","<;:9876>","<;:9876>?","<;:9876>?@","<;:9876>?@A","<;:9876>?@AB","<;:9876>?@ABC","<;:9876>?D","<;:9876>?DE","<;:9876>?DEF","<;:9876>?DEFG","<;:987653IH","<;:987653I","<;:987653IJ","<;:98765K","<;:9876>?DEL"},
    {"=<;:987653210","=<;:98765321","=<;:9876532","=<;:987653","=<;:987654","=<;:98765","=<;:9876","=<;:987","=<;:98","=<;:9","=<;:","=<;","=<","=","=<;:9876>","=<;:9876>?","=<;:9876>?@","=<;:9876>?@A","=<;:9876>?@AB","=<;:9876>?@ABC","=<;:9876>?D","=<;:9876>?DE","=<;:9876>?DEF","=<;:9876>?DEFG","=<;:987653IH","=<;:987653I","=<;:987653IJ","=<;:98765K","=<;:9876>?DEL"},
    {">653210",">65321",">6532",">653",">654",">65",">6",">67",">678",">6789",">6789:",">6789:;",">6789:;<",">6789:;<=",">",">?",">?@",">?@A",">?@AB",">?@ABC",">?D",">?DE",">?DEF",">?DEFG",">?DEFGH",">653I",">653IJ",">65K",">?DEL"},
    {"?>653210","?>65321","?>6532","?>653","?>654","?>65","?>6","?>67","?>678","?>6789","?>6789:","?>6789:;","?>6789:;<","?>6789:;<=","?>","?","?@","?@A","?@AB","?@ABC","?D","?DE","?DEF","?DEFG","?DEFGH","?>653I","?>653IJ","?>65K","?DEL"},
    {"@?>653210","@?>65321","@?>6532","@?>653","@?>654","@?>65","@?>6","@?>67","@?>678","@?>6789","@?>6789:","@?>6789:;","@?>6789:;<","@?>6789:;<=","@?>","@?","@","@A","@AB","@ABC","@ED","@E","@EF","@EFG","@EFGH","@?>653I","@?>653IJ","@?>65K","@EL"},
    {"A@?>653210","A@?>65321","A@?>6532","A@?>653","A@?>654","A@?>65","A@?>6","A@?>67","A@?>678","A@?>6789","A@?>6789:","A@?>6789:;","A@?>6789:;<","A@?>6789:;<=","A@?>","A@?","A@","A","AB","ABC","AED","AE","AEF","AEFG","AEFGH","A@?>653I","A@?>653IJ","A@?>65K","AEL"},
    {"BA@?>653210","BA@?>65321","BA@?>6532","BA@?>653","BA@?>654","BA@?>65","BA@?>6","BA@?>67","BA@?>678","BA@?>6789","BA@?>6789:","BA@?>6789:;","BA@?>6789:;<","BA@?>6789:;<=","BA@?>","BA@?","BA@","BA","B","BC","BAED","BAE","BAEF","BAEFG","BAEFGH","BA@?>653I","BA@?>653IJ","BA@?>65K","BAEL"},
    {"CBA@?>653210","CBA@?>65321","CBA@?>6532","CBA@?>653","CBA@?>654","CBA@?>65","CBA@?>6","CBA@?>67","CBA@?>678","CBA@?>6789","CBA@?>6789:","CBA@?>6789:;","CBA@?>6789:;<","CBA@?>6789:;<=","CBA@?>","CBA@?","CBA@","CBA","CB","C","CBAED","CBAE","CBAEF","CBAEFG","CBAEFGH","CBA@?>653I","CBA@?>653IJ","CBA@?>65K","CBAEL"},
    {"D?>653210","D?>65321","D?>6532","D?>653","D?>654","D?>65","D?>6","D?>67","D?>678","D?>6789","D?>6789:","D?>6789:;","D?>6789:;<","D?>6789:;<=","D?>","D?","DE@","DEA","DEAB","DEBC","D","DE","DEF","DEFG","DEFGH","D?>653I","D?>653IJ","D?>65K","DEL"},
    {"ED?>653210","ED?>65321","ED?>6532","ED?>653","ED?>654","ED?>65","ED?>6","ED?>67","ED?>678","ED?>6789","ED?>6789:","ED?>6789:;","ED?>6789:;<","ED?>6789:;<=","ED?>","ED?","E@","EA","EAB","EBC","ED","E","EF","EFG","EFGH","EFGHI","EFGHIJ","ED?>65K","EL"},
    {"FGHIJ10","FGHIJ1","FGHI32","FGHI3","FGHI34","FGHI345","FED?>6","FED?>67","FED?>678","FED?>6789","FED?>6789:","FED?>6789:;","FED?>6789:;<","FED?>6789:;<=","FED?>","FED?","FE@","FEA","FEAB","FEBC","FED","FE","F","FG","FGH","FGHI","FGHIJ","FGHI35K","FL"},
    {"GHIJ10","GHIJ1","GHI32","GHI3","GHI34","GHI345","GFED?>6","GFED?>67","GFED?>678","GFED?>6789","GFED?>6789:","GFED?>6789:;","GFED?>6789:;<","GFED?>6789:;<=","GFED?>","GFED?","GFE@","GFEA","GFEAB","GFEBC","GFED","GFE","GF","G","GH","GHI","GHIJ","GHI35K","GFL"},
    {"HIJ10","HIJ1","HI32","HI3","HI34","HI345","HI3456","HI34567","HI345678","HI3456789","HI3456789:","HI3456789:;","HI3456789:;<","HI3456789:;<=","HI3456>","HGFED?","HGFE@","HGFEA","HGFEAB","HGFEBC","HGFED","HGFE","HGF","HG","H","HI","HIJ","HI35K","HGFL"},
    {"IJ10","IJ1","I32","I3","I34","I345","I3456","I34567","I345678","I3456789","I3456789:","I3456789:;","I3456789:;<","I3456789:;<=","I3456>","IHGFED?","IHGFE@","IHGFE@A","IHGFE@AB","IHGFE@ABC","IHGFED","IHGFE","IHGF","IHG","IH","I","IJ","I35K","IHGFL"},
    {"J10","J1","JI32","JI3","JI34","JI345","JI3456","JI34567","JI345678","JI3456789","JI3456789:","JI3456789:;","JI3456789:;<","JI3456789:;<=","JI3456>","JIHGFED?","JIHGFE@","JIHGFE@A","JIHGFE@AB","JIHGFE@ABC","JIHGFED","JIHGFE","JIHGF","JIHG","JIH","JI","J","JI35K","JIHGFL"},
    {"K210","K21","K53","K54","K5","K56","K567","K5678","K56789","K56789:","K56789:;","K56789:;<","K56789:;<=","K56>","K56>?","K56>?@","K56>?@A","K56>?@AB","K56>?@ABC","K56>?D","K56>?DE","K53IHGF","K53IHG","K53IH","K53I","K53IJ","K","K56>?DEL"},
    {"LFGHIJ10","LFGHIJ1","LFGHI32","LFGHI3","LFGHI34","LED?>65","LED?>6","LED?>67","LED?>678","LED?>6789","LED?>6789:","LED?>6789:;","LED?>6789:;<","LED?>6789:;<=","LED?>","LED?","LE@","LEA","LEAB","LEABC","LED","LE","LF","LFG","LFGH","LFGHI","LFGHIJ","LED?>65K","L"}
};
const char* TB1path2[16][16] = {
    {"0","01","012","013","04","045","0456","0457","08","089","089:","089;","04<","04<=","04<=>","04<=?"},
    {"10","1","12","13","104","1365","10456","10457","108","1089","1089:","12:9;","104<","104<=","104<=>","104<=?"},
    {"210","21","2","213","2104","21365","210456","210457","2:98","2:9","2:","2:9;","2104<","2104<=","2104<=>","2104<=?"},
    {"310","31","32","3","3104","365","36","3657","3108","32:9","32:","32:9;","32104<","32104<=","32104<=>","32104<=?"},
    {"40","401","4012","4013","4","45","456","457","408","4089","4089:","4089;","4<","4<=","4<=>","4<=?"},
    {"540","5401","54012","54013","54","5","56","57","5408","54089","54089:","54089;","54<","57?=","57?=>","57?"},
    {"6310","631","632","63","654","65","6","67","63108","6312:9","632:","632:;","654<","657?=","657?=>","657?"},
    {"7540","75631","756312","7563","754","75","76","7","75408","754089","754089:","754089;","7?=<","7?=","7?=>","7?"},
    {"80","801","8012","8013","804","8045","80456","80457","8","89","89:","89;","8<","8<=","8<=>","8<=?"},
    {"980","9:21","9:2","9:23","9804","98045","980136","980457","98","9","9:","9;","98<","98<=","98<=>","98<=?"},
    {":210",":21",":2",":23",":2104",":21045",":236",":2367",":98",":9",":",":;",":98<",":9;>=",":;>",":;>?"},
    {";980",";:21",";:2",";:23",";9804",";98045",";980456",";980457",";98",";9",";:",";",";>=<",";>=",";>",";>?"},
    {"<80","<801","<89:2","<8013","<4","<45","<456","<457","<8","<89","<89:","<89;","<","<=","<=>","<=?"},
    {"=<80","=<801","=<89:2","=<8013","=<4","=?75","=?76","=?7","=<8","=>;9","=>;:","=>;","=<","=","=>","=?"},
    {">;980",">;9:21",">;9:2",">;9:23",">=<4",">=?75",">=?76",">=?7",">;98",">;9",">;:",">;",">=<",">=",">",">?"},
    {"?=<40","?=>401","?=<4012","?=<4013","?754","?75","?76","?7","?=<8","?>;9","?>;:","?>;","?=<","?=","?>","?"},

};

//保存各个教室的窗子坐标（待补充）
WindowNode TB3window[4] = { {25,10,27,10},{17,22,19,22},{13,20,11,20}, {14,5,14,3} };
WindowNode TB5window[3] = { {17,31,17,29},{15,42,15,44},{6,32,4,32} };
WindowNode TB6window[5] = { {40,17,42,17},{32,18,32,16},{27,34,25,34},{36,42,38,42},{45,37,45,39} };
WindowNode TB1window[12] = { 
    {27,10,25,10},{19,22,17,22},{11,20,13,20}, {14,3,14,5} ,
    {17,29,17,31},{15,44,15,42},{4,32,6,32} ,{42,17,40,17},
    {32,16,32,18},{25,34,27,34},{38,42,36,42},{45,39,45,37} };
const int TB3windowNum = 4;
const int TB5windowNum = 3;
const int TB6windowNum = 5;
const int TB1windowNum = 12;

//各个教室的作业信息（待补充）
Classroom TB3[3] = {
    {0,0,{{31,6},{30,6},{29,7},{30,8}},{30,7}} ,
    {1,0,{{18,4},{17,5},{18,6},{19,5}},{18,5}},
    {2,0,{{22,17},{21,18},{22,19},{23,18}},{22,18}} };
Classroom TB5[3] = { 
    {3,0,{{8,30},{7,31},{8,32},{9,31}}, {8,31}},
    {4,0,{{10,37},{9,38},{10,39},{9,39}}, {10,38}},
    {5,0,{{19,40},{18,41},{19,42},{18,40}},{19,41} } };
Classroom TB6[4] = { 
    {6,0,{{39,12},{39,12},{40,13},{40,13}},{40,12}},
    {7,0,{{28,25},{27,26},{28,27},{29,27}}, {28,26}},
    {8,0,{{44,31},{43,32},{44,33},{45,32}}, {44,32}},
    {9,0,{{33,39},{32,40},{33,41},{34,40}}, {33,40} } };
Classroom* AllClassroom[10] = { &TB3[0], &TB3[1], &TB3[2],
                                 &TB5[0], &TB5[1], &TB5[2],
                                 &TB6[0], &TB6[1], &TB6[2], &TB6[3] };

//各个大门的信息（待补充）
Gate gate[6] = { {0,{5,6},1},{0,{46,45},1}, {0,{34,5},0}, {0,{47,13},0}, {0,{2,23},0}, {0,{21,47},0} };

enum class BulletType : unsigned char
{
    NullBulletType = 0,
    FlyingKnife = 1,
    CommonAttackOfTricker = 2,
    BombBomb = 3,
    JumpyDumpty = 4,
    AtomBomb = 5,
};
//捣蛋鬼攻击类型数组
int AttackRange[6] = { 0,78000,2200,2200,1600,2000 };

static bool Arrived = false;
static bool Arrived1 = false;
int lastSkipCount = 0;

StudentStatus Stu1Status = StudentStatus::Init;//定义玩家的初始状态为Init
TrickerStatus Tricker1Status = TrickerStatus::Tracing;
int NowArea=0;
MapNode FinalDestination={0,0};//目前只在SetPath()中会更改
MapNode ClosedStudent = { 0,0 };//单位是grid，不是cell
MapNode TempDestination;//暂时好像还没用到，在某些函数中用局部变量来替代它，好像不需要作为全局变量
Classroom* TempClassroom;//在WantToStudy状态下会用到，其实也是一个不需要作为全局变量的变量
Gate* TempGate = &gate[0];//在WantToGraduate状态和Graduating状态下会用到。
int FinishedClassroom = 0;//已完成的作业数
int TB3FinishedClassroom = 0;
int TB5FinishedClassroom = 0;
int TB6FinishedClassroom = 0;

int lastStayCount = 0;
MapNode lastposition = { 0,0 };
MapNode lastposition1 = { 0,0 };
bool Stuck = false;
MapNode KeyNode[3] = { {25,7},{8,35}, {40,25} };//存放的是Tricker选择TB时的目的地关键点，不和之前定义的TBNode重复

//Tricker用于Searching的ClosedPath
MapNode ClosedTrickerPath[56] = {
    {20,25},{19,22},{17,22},{17,16},{21,16},{22,7},{20,4},{20,2},{20,3},{20,5},
    {29,4},{30,3},{29,4},{24,8},{25,10},{28,11},{32,16},{32,24},{40,19},{40,30},
    {45,30},{45,37},{43,43},{43,42},{39,38},{39,39},{38,39},{39,39},{38,42},{33,36},
    {27,34},{24,34},{23,41},{21,44},{21,46},{16,46},{16,45},{15,46},{15,42},{19,39},
    {20,36},{17,33},{14,36},{9,36},{7,32},{6,32},{4,34},{5,34},{4,34},{4,21},
    {9,21},{6,25},{14,25},{14,29},{14,28}, {17,25},
};
MapNode ClosedTrickerPath2[68] = {
    {17,21},{18,19},{30,18},{31,30},{19,31},{21,36},{17,38},{12,33},{11,38},{6,43},
    {4,41},{11,48},{20,48},{20,44},{15,45},{19,42},{30,42},{34,45},{31,43},{31,42},
    {31,39},{37,33},{38,38},{43,43},{41,45},{48,38},{48,29},{44,29},{45,34},{42,30},
    {42,29},{42,19},{45,15},{43,18},{42,18},{39,18},{33,12},{38,11},{43,6},{46,9},
    {38,1},{29,1},{29,5},{34,4},{32,7},{30,7},{17,7},{15,4},{18,6},{18,5},
    {18,10},{12,16},{11,11},{6,6},{8,4},{1,11},{1,20},{5,20},{4,15},{7,17},
    {7,19},{7,32},{4,34},{6,31},{7,31},{11,31},{11,20},{9,21},
};
int Closedpathlength = 56;
int Closedpathlength2 = 68;
bool StatusIsSearching = false;

//用于学生的固定去往settedpoint的路径
MapNode Pathfor0[5] = { {36,7},{36,11}, {36,18},{38,18},{40,13} };
MapNode Pathfor1[3] = { {3,37},{8,37},{10,37} };
MapNode Pathfor2[4] = { {3,10},{10,13},{13,13},{17,5} };
MapNode Pathfor3[4] = { {39,38},{40,35},{44,35},{44,33} };
MapNode* Ppath[4] = {Pathfor0,Pathfor1,Pathfor2,Pathfor3};

MapNode Pathfor02[7] = { {4,2},{4,4},{11,11},{11,18},{9,18},{6,18},{6,14} };
MapNode Pathfor22[7] = { {45,2},{45,4},{38,11},{31,11},{31,9},{31,6},{35,6} };
MapNode Pathfor12[7] = { {4,47},{4,45},{11,38},{11,31},{9,31},{6,31},{6,35} };
MapNode Pathfor32[7] = { {45,47},{45,45},{38,38},{38,31},{40,31},{43,31},{43,35} };
MapNode* Ppath2[4] = { Pathfor02,Pathfor12,Pathfor22,Pathfor32 };
int PathforLength[4] = { 5,3,4,4 };
int PathforLength2[4] = { 7,7,7,7 };


/*-------------------------------自定义的函数声明-------------------------------*/

/*--------------------------------基础功能函数----------------------------------*/

static inline int CellToGrid(int cell) noexcept;//·返回格子的中心坐标
static inline int GridToCell(int grid) noexcept;//·返回该坐标所处的格子
int GetArea(int x, int y);//·返回所在区域的值
double GetDistance(double x1, double y1, double x2, double y2);//·返回两点间的距离

/*----------------------------------用于移动------------------------------------*/

void Goto(ITrickerAPI& api, int destX, int destY);//·去到一个格点坐标（不考虑是否有墙）(For Tricker)
void GotoGrid(ITrickerAPI& api, double destX, double destY);//·去到一个Grid（For Tricker)
void Goto(IStudentAPI& api, int destX, int destY);//·去到一个格点坐标（不考虑是否有墙）(For Student)
void GotoGrid(IStudentAPI& api, double destX, double destY);//·去到一个Grid(For Tricker)
bool arriveAt(IStudentAPI &api, int x2, int y2);//·判断是否到达某点(For Student)
bool arriveAt(ITrickerAPI& api, int x2, int y2);//·判断是否到达某点
bool isWalled(int x1, int y1, int x2, int y2);//·判断两点之间是否有墙
int MostCloseNode(int x, int y, int AreaNum);//·寻找在该区域内离这个点最近的结点
const char* PathInAnArea(IAPI& api, int origX, int origY, int destX, int destY, int AreaNum);//·在一个区域内，返回路径(字符串)
void SetPath(IAPI &api, int origX,int origY,int destX, int destY);//·把path数组进行修改,也更改全局变量FinalDestination
bool MoveAlongPath(IStudentAPI &api);//·用于沿着path移动的函数，到达目的地时返回ture(For Student)
bool MoveAlongPath(ITrickerAPI& api);//·用于沿着path移动的函数，到达目的地时返回ture(For Tricker)
                                     //缺一个更新，终点是作业的情况

/*------------------------寻找教室、窗子、宝箱、大门-----------------------------*/

int MostCloseWindow(int x, int y, int AreaNum);//·寻找在该区域内离这个点最近的窗子
Classroom* ChooseAClassroom(IStudentAPI& api, int x, int y,int ID);//·寻找未写完的最近的作业(现在同一个区域找，若没有，在所有区域找）

Gate* ChooseAGate(IStudentAPI& api, int x, int y);//寻找离得最近的校门（包括隐藏校门）

/*---------------------------用于初始化以及决策----------------------------------*/

void InitMap(IAPI &api);//·扫描地图，获取每个格子的placetype，初始化全局变量map与nodemap
void SetStatus(ITrickerAPI& api);//用于设置状态(For Tricker)
void SetStatus(IStudentAPI& api);//用于设置状态(For Student)
void UpdateInfo(IStudentAPI& api);//用于刷新信息
void UpdateInfo(ITrickerAPI& api, int ID);//用于刷新信息，使用了GetGameInfo来获取写完的作业数，但在Student的函数里还没改

int MostCloseStudent(ITrickerAPI& api);//返回-1说明视野内没有血的学生
bool BeStuck(ITrickerAPI& api);
bool BeStuck(IStudentAPI& api);
bool BeLongStuck(ITrickerAPI& api);

//函数定义

static inline int CellToGrid(int cell) noexcept
{
    return cell * numOfGridPerCell + numOfGridPerCell / 2;
}
static inline int GridToCell(int grid) noexcept
{
    return grid / numOfGridPerCell;
}
int GetArea(int x, int y)
{
    if ((x >= 17 && x <= 20 && y >= 1 && y <= 3) || (x >= 10 && x <= 25 && y >= 4 && y <= 11) || (x >= 26 && x <= 31 && y >= 3 && y <= 9)
        || (x >= 12 && x <= 25 && y >= 12 && y <= 20) || (x >= 26 && x <= 27 && y >= 14 && y <= 16) || (x >= 12 && x <= 17 && y >= 21 && y <= 22))
        return 3;
    else if ((x >= 6 && x <= 9 && y >= 27 && y <= 42) || (x >= 10 && x <= 12 && y >= 35 && y <= 46) || (x >= 13 && x <= 20 && y >= 35 && y <= 42)
        || (x >= 17 && x <= 19 && y >= 43 && y <= 44) || (x >= 15 && x <= 21 && y >= 30 && y <= 36) || (x == 19 && y >= 28 && y <= 29))
        return 5;
    else if ((x >= 40 && x <= 41 && y >= 9 && y <= 11) || (x >= 38 && x <= 41 && y >= 12 && y <= 16) || (x == 42 && y == 19) || (x >= 32 && x <= 41 && y >= 17 && y <= 23)
        || (x >= 29 && x <= 30 && y == 23) || (x >= 27 && x <= 46 && y >= 23 && y <= 32) || (x >= 27 && x <= 37 && y >= 33 && y <= 39) || (x >= 43 && x <= 48 && y >= 33 && y <= 37) || (x >= 30 && x <= 36 && y >= 40 && y <= 43))
        return 6;
    else return 1;
}
double GetDistance(double x1, double y1, double x2, double y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void Goto(ITrickerAPI& api, int destX, int destY)
{
    api.Print(fmt::format("Goto({},{})", destX, destY));
    auto self = api.GetSelfInfo();
    int selfx = self->x;
    int selfy = self->y;
    auto delta_x = (double)(CellToGrid(destX) - selfx);
    auto delta_y = (double)(CellToGrid(destY) - selfy);
    double ang = 0;
    ang = atan2(delta_y, delta_x);


    api.Move(200, ang);
}
void GotoGrid(ITrickerAPI& api, double destX, double destY)
{
    api.Print(fmt::format("GotoGrid({},{})", destX, destY));
    auto self = api.GetSelfInfo();
    int selfx = self->x;
    int selfy = self->y;
    auto delta_x = destX - selfx;
    auto delta_y = destY - selfy;
    double ang = 0;
    ang = atan2(delta_y, delta_x);


    api.Move(800, ang);
}
void Goto(IStudentAPI& api, int destX, int destY)

{
    api.Print(fmt::format("Goto({},{})", destX, destY));
    auto self = api.GetSelfInfo();
    int selfx = self->x;
    int selfy = self->y;
    auto delta_x = (double)(CellToGrid(destX) - selfx);
    auto delta_y = (double)(CellToGrid(destY) - selfy);
    double ang = 0;
    ang = atan2(delta_y, delta_x);
    api.Print(fmt::format("My location:{},{}", GridToCell(self->x), GridToCell(self->y)));
    api.Print(fmt::format("delta_x+delta_y:{}", delta_x + delta_y));
    api.Print(fmt::format("angle:", ang));
    api.Move(200, ang);
    //api.EndAllAction();
}
void GotoGrid(IStudentAPI& api, double destX, double destY)
{
    api.Print(fmt::format("GotoGrid({},{})", destX, destY));
    auto self = api.GetSelfInfo();
    int selfx = self->x;
    int selfy = self->y;
    auto delta_x = destX - selfx;
    auto delta_y = destY - selfy;
    double ang = 0;
    ang = atan2(delta_y, delta_x);


    api.Move(400, ang );
}
bool arriveAt(IStudentAPI &api,int x2, int y2)
{
    auto self = api.GetSelfInfo();
    int selfx = self->x;
    int selfy = self->y;
    if (std::abs(selfx - (1000 * x2+500)) + std::abs(selfy- (1000 * y2+500)) < 200)
    {
        return true;
    }
    return false;
}
bool arriveAt(ITrickerAPI& api, int x2, int y2)
{
    auto self = api.GetSelfInfo();
    int selfx = self->x;
    int selfy = self->y;
    if (std::abs(selfx - (1000 * x2 + 500)) + std::abs(selfy - (1000 * y2 + 500)) < 200)
    {
        return true;
    }
    return false;
}
bool arriveAtMax(IStudentAPI& api, int x2, int y2)
{
    auto self = api.GetSelfInfo();
    int selfx = self->x;
    int selfy = self->y;
    if (std::abs(selfx - (1000 * x2 + 500)) + std::abs(selfy - (1000 * y2 + 500)) < 1800)
    {
        return true;
    }
    return false;
}
bool arriveAtMax(ITrickerAPI& api, int x2, int y2)
{
    auto self = api.GetSelfInfo();
    int selfx = self->x;
    int selfy = self->y;
    if (std::abs(selfx - (1000 * x2 + 500)) + std::abs(selfy - (1000 * y2 + 500)) < 1200)
    {
        api.Print("ArrivedMax");
        return true;
    }
    api.Print("NotArrivedMax");
    return false;
}
bool isWalled1(int x1, int y1, int x2, int y2)
{
    int ktype = 2;//0对于斜率不存在，1对于斜率为0, 2对于斜率非0
    double k, b;
    if (x1 == x2)
        ktype = 0;
    else if (y1 == y2)
        ktype = 1;
    switch (ktype)
    {
    case 0:
    {if (y1 > y2)
    {
        for (int y = y2 + 1; y < y1; y++)
        {
            if (map[x1][y] == THUAI6::PlaceType::Wall || map[x1][y] == THUAI6::PlaceType::Chest || map[x1][y] == THUAI6::PlaceType::ClassRoom)
                return true;
        }
        return false;
    }
    else
    {
        for (int y = y1 + 1; y < y2; y++)
        {
            if (map[x1][y] == THUAI6::PlaceType::Wall || map[x1][y] == THUAI6::PlaceType::Chest || map[x1][y] == THUAI6::PlaceType::ClassRoom)
                return true;
        }
        return false;
    }
    break; }
    case 1:
    {if (x1 > x2)
    {
        for (int x = x2 + 1; x < x1; x++)
        {
            if (map[x][y1] == THUAI6::PlaceType::Wall || map[x][y1] == THUAI6::PlaceType::Chest || map[x][y1] == THUAI6::PlaceType::ClassRoom)
                return true;
        }
        return false;
    }
    else
    {
        for (int x = x1 + 1; x < x2; x++)
        {
            if (map[x][y1] == THUAI6::PlaceType::Wall || map[x][y1] == THUAI6::PlaceType::Chest || map[x][y1] == THUAI6::PlaceType::ClassRoom)
                return true;
        }
        return false;
    }
    break; }
    case 2:
    {k = (y1 - y2) / (double)(x1 - x2);
    b = (y1 * x2 - y2 * x1) / (double)(x2 - x1);
    double x, y;
    if (x1 < x2)
    {
        for (x = x1 ; x < x2; x += 0.01)
        {
            y = k * x + b;
            int xGrid = CellToGrid(x);
            int yGrid = CellToGrid(y);
            int xCell = GridToCell(xGrid);
            int yCell = GridToCell(yGrid);
            if (map[xCell][yCell] == THUAI6::PlaceType::Wall || map[xCell][yCell] == THUAI6::PlaceType::Chest || map[xCell][yCell] == THUAI6::PlaceType::ClassRoom)
                return true;
        }
        return false;
    }
    else
    {
        for (x = x2; x < x1 ; x += 0.01)
        {
            y = k * x + b;
            int xGrid = CellToGrid(x);
            int yGrid = CellToGrid(y);
            int xCell = GridToCell(xGrid);
            int yCell = GridToCell(yGrid);
            if (map[xCell][yCell] == THUAI6::PlaceType::Wall || map[xCell][yCell] == THUAI6::PlaceType::Chest || map[xCell][yCell] == THUAI6::PlaceType::ClassRoom)
                return true;
        }
        return false;
    }}
    default:
        return false;
        break;
    }
}
bool isWalled(int x1, int y1, int x2, int y2)
{
    int ktype = 2;//0对于斜率不存在，1对于斜率为0, 2对于斜率非0
    double k, b;
    if (x1 == x2)
        ktype = 0;
    else if (y1 == y2)
        ktype = 1;
    switch (ktype)
    {
    case 0:
    {if (y1 > y2)
    {
        for (int y = y2 + 1; y < y1; y++)
        {
            if (map[x1][y] == THUAI6::PlaceType::Wall || map[x1][y] == THUAI6::PlaceType::Chest || map[x1][y] == THUAI6::PlaceType::ClassRoom)
                return true;
        }
        return false;
    }
    else
    {
        for (int y = y1 + 1; y < y2; y++)
        {
            if (map[x1][y] == THUAI6::PlaceType::Wall || map[x1][y] == THUAI6::PlaceType::Chest || map[x1][y] == THUAI6::PlaceType::ClassRoom)
                return true;
        }
        return false;
    }
    break; }
    case 1:
    {if (x1 > x2)
    {
        for (int x = x2 + 1; x < x1; x++)
        {
            if (map[x][y1] == THUAI6::PlaceType::Wall || map[x][y1] == THUAI6::PlaceType::Chest || map[x][y1] == THUAI6::PlaceType::ClassRoom)
                return true;
        }
        return false;
    }
    else
    {
        for (int x = x1 + 1; x < x2; x++)
        {
            if (map[x][y1] == THUAI6::PlaceType::Wall || map[x][y1] == THUAI6::PlaceType::Chest || map[x][y1] == THUAI6::PlaceType::ClassRoom)
                return true;
        }
        return false;
    }
    break; }
    case 2:
    {
        int x1Grid = CellToGrid(x1);
        int y1Grid = CellToGrid(y1);
        int x2Grid = CellToGrid(x2);
        int y2Grid = CellToGrid(y2);
        k = (y1Grid - y2Grid) / (double)(x1Grid - x2Grid);
        b = (y1Grid * x2Grid - y2Grid * x1Grid) / (double)(x2Grid - x1Grid);
    double x, y;
    if (x1 < x2)
    {
        for (x = x1Grid; x < x2Grid; x += 100)
        {
            y = k * x + b;
            int xCell = GridToCell(x);
            int yCell = GridToCell(y);
            if (map[xCell][yCell] == THUAI6::PlaceType::Wall || map[xCell][yCell] == THUAI6::PlaceType::Chest || map[xCell][yCell] == THUAI6::PlaceType::ClassRoom)
                return true;
        }
        return false;
    }
    else
    {
        for (x = x2Grid; x < x1Grid; x += 0.01)
        {
            y = k * x + b;
            
            int xCell = GridToCell(x);
            int yCell = GridToCell(y);
            if (map[xCell][yCell] == THUAI6::PlaceType::Wall || map[xCell][yCell] == THUAI6::PlaceType::Chest || map[xCell][yCell] == THUAI6::PlaceType::ClassRoom)
                return true;
        }
        return false;
    }}
    default:
        return false;
        break;
    }
}
int MostCloseNode(int x, int y, int AreaNum,MapNode dest)
{
    MapNode* areanode;
    int nodenum, result = -1;
    switch (AreaNum)//选择合适的教室节点图
    {
    case 3:
    {areanode = TB3node;
    nodenum = sizeof_TB3node;
    break; }
    case 5:
    {areanode = TB5node;
    nodenum = sizeof_TB5node;
    break; }
    case 6:
    {areanode = TB6node;
    nodenum = sizeof_TB6node;
    break; }
    case 1:
    {areanode = TB1node;
    nodenum = sizeof_TB1node;
    break; }
    default:
    {areanode = { 0 };
    nodenum = 0;
    break; }
    }
    double dis, min_dis = 1000;
    for (int i = 0; i < nodenum; i++)
    {
        double x_p = areanode[i].x;
        double y_p = areanode[i].y;
        dis = sqrt((x - x_p) * (x - x_p) + (y - y_p) * (y - y_p))+GetDistance(x_p,y_p,dest.x,dest.y);
        if (dis < min_dis)
        {
            if (!isWalled(x, y, x_p, y_p))
            {
                min_dis = dis;
                result = i;
            }
        }
    }
    return result;
}//寻找在该区域内离这个点最近的结点
int MostCloseNode2(int x, int y, int AreaNum, MapNode dest)
{
    MapNode* areanode;
    int nodenum, result = -1;
    switch (AreaNum)//选择合适的教室节点图
    {
    case 3:
    {areanode = TB3node;
    nodenum = sizeof_TB3node;
    break; }
    case 5:
    {areanode = TB5node;
    nodenum = sizeof_TB5node;
    break; }
    case 6:
    {areanode = TB6node;
    nodenum = sizeof_TB6node;
    break; }
    case 1:
    {areanode = TB1node2;
    nodenum = sizeof_TB1node2;
    break; }
    default:
    {areanode = { 0 };
    nodenum = 0;
    break; }
    }
    double dis, min_dis = 1000;
    for (int i = 0; i < nodenum; i++)
    {
        double x_p = areanode[i].x;
        double y_p = areanode[i].y;
        dis = sqrt((x - x_p) * (x - x_p) + (y - y_p) * (y - y_p)) + GetDistance(x_p, y_p, dest.x, dest.y);
        if (dis < min_dis)
        {
            if (!isWalled(x, y, x_p, y_p))
            {
                min_dis = dis;
                result = i;
            }
        }
    }
    return result;
}//寻找在该区域内离这个点最近的结点
const char* PathInAnArea(IAPI& api, int origX, int origY, int destX, int destY, int AreaNum)
{
    //先根据AreaNum的值，找离起点、终点最近的点的序号
    MapNode* areanode;
    int nodenum;
    switch (AreaNum)//选择合适的教室节点图
    {
    case 3:
    {areanode = TB3node;
    nodenum = sizeof_TB3node;
    break; }
    case 5:
    {areanode = TB5node;
    nodenum = sizeof_TB5node;
    break; }
    case 6:
    {areanode = TB6node;
    nodenum = sizeof_TB6node;
    break; }
    case 1:
    {areanode = TB1node;
    nodenum = sizeof_TB1node;
    break; }
    default:
    {areanode = { 0 };
    nodenum = 0;
    break; }
    }
    int from = MostCloseNode(origX, origY, AreaNum, MapNode{ destX,destY });
    int to = MostCloseNode(destX, destY, AreaNum, MapNode{ origX,origY });
    api.Print("PathInAnArea has been called");
    api.Print(fmt::format("FromTB{}[{}]", AreaNum,from));
    api.Print(fmt::format("ToTB{}[{}]", AreaNum, to));
    //再根据AreaNum的值，在对应的“结点to结点路径数组”中找到该路径(字符串）
    switch (AreaNum)
    {
    case 3:
    {return TB3path[from][to]; }
    case 5:
    {return TB5path[from][to]; }
    case 6:
    {return TB6path[from][to]; }
    case 1:
    {return TB1path[from][to]; }
    default:
    {return TB3path[0][0];
        break; }
    }
    //返回该路径（字符串）
}
const char* PathInAnArea2(IAPI& api, int origX, int origY, int destX, int destY, int AreaNum)
{
    //先根据AreaNum的值，找离起点、终点最近的点的序号
    MapNode* areanode;
    int nodenum;
    switch (AreaNum)//选择合适的教室节点图
    {
    case 3:
    {areanode = TB3node;
    nodenum = sizeof_TB3node;
    break; }
    case 5:
    {areanode = TB5node;
    nodenum = sizeof_TB5node;
    break; }
    case 6:
    {areanode = TB6node;
    nodenum = sizeof_TB6node;
    break; }
    case 1:
    {areanode = TB1node2;
    nodenum = sizeof_TB1node2;
    break; }
    default:
    {areanode = { 0 };
    nodenum = 0;
    break; }
    }
    int from = MostCloseNode2(origX, origY, AreaNum, MapNode{ destX,destY });
    int to = MostCloseNode2(destX, destY, AreaNum, MapNode{ origX,origY });
    api.Print("PathInAnArea has been called");
    api.Print(fmt::format("FromTB{}[{}]", AreaNum, from));
    api.Print(fmt::format("ToTB{}[{}]", AreaNum, to));
    //再根据AreaNum的值，在对应的“结点to结点路径数组”中找到该路径(字符串）
    switch (AreaNum)
    {
    case 3:
    {return TB3path[from][to]; }
    case 5:
    {return TB5path[from][to]; }
    case 6:
    {return TB6path[from][to]; }
    case 1:
    {return TB1path2[from][to]; }
    default:
    {return TB3path[0][0];
    break; }
    }
    //返回该路径（字符串）
}
void SetPath(IAPI& api, int origX, int origY, int destX, int destY)
{
    int origArea = GetArea(origX, origY);
    int destArea = GetArea(destX, destY);
    if (origArea == destArea)
    {
        const char* path1 = PathInAnArea(api, origX, origY, destX, destY, origArea);
        MapNode* areanode;
        switch (origArea)//选择合适的教室节点图
        {
        case 3:
        {areanode = TB3node;
        break; }
        case 5:
        {areanode = TB5node;
        break; }
        case 6:
        {areanode = TB6node;
        break; }
        case 1:
        {areanode = TB1node;
        break; }
        default:
        {areanode = { 0 };
        break; }
        }
        int i = 0;
        for (i = 0; i < strlen(path1); i++)//把path1（字符串）中的路径转为path（结点数组）的元素
        {
            path[i] = areanode[(int)path1[i] - 48];
            pathlength++;
        }
        path[i] = { destX,destY };//把真实要去的点作为最后一个节点
        pathlength++;

    }
    else//如果不在一间教室的情况
    {
        if (origArea == 3 && destArea == 5)//三教到五教
        {
            const char* path3to5_3 = PathInAnArea(api, origX, origY, 17, 22, 3);
            const char* path3to5_5 = PathInAnArea(api, 17, 31, destX, destY, 5);
            api.Print(path3to5_3);
            api.Print(path3to5_5);
            int i = 0, j = 0;
            int length = strlen(path3to5_3) + strlen(path3to5_5);
            for (i = 0; i < strlen(path3to5_3); i++)
            {
                path[i] = TB3node[(int)path3to5_3[i] - 48];
                pathlength++;
            }
            path[i] = { 17,22 };
            path[i + 1] = { 19,25 };
            path[i + 2] = { 17,25 };
            path[i + 3] = { 17,29 };
            pathlength += 4;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(path3to5_5); i++, j++, pathlength++)
            {
                path[i] = TB5node[(int)path3to5_5[j] - 48];
            }
            pathlength = length + 5;
            path[pathlength - 1] = { destX,destY };

        }
        else if (origArea == 5 && destArea == 3)//五教到三教
        {
            const char* path5to3_5 = PathInAnArea(api, origX, origY, 17, 31, 5);
            const char* path5to3_3 = PathInAnArea(api, 17, 22, destX, destY, 3);
            int i = 0, j = 0;
            int length = strlen(path5to3_3) + strlen(path5to3_5);
            for (i = 0; i < strlen(path5to3_5); i++)
            {
                path[i] = TB5node[(int)path5to3_5[i] - 48];
                pathlength++;
            }
            path[i] = { 17,31 };
            path[i + 1] = { 17,29 };
            path[i + 2] = { 17,25 };
            path[i + 3] = { 19,25 };
            path[i + 4] = { 19,22 };
            path[i + 5] = { 17,22 };
            pathlength += 6;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(path5to3_3); i++, j++, pathlength++)
            {
                path[i] = TB3node[(int)path5to3_3[j] - 48];
            }
            pathlength = length + 7;
            path[pathlength - 1] = { destX,destY };

        }
        else if (origArea == 3 && destArea == 6)//三教到六教
        {
            const char* path6to3_3 = PathInAnArea(api, origX, origY, 25, 10, 3);
            const char* path6to3_6 = PathInAnArea(api, 32, 18, destX, destY, 6);
            int i = 0, j = 0;
            int length = strlen(path6to3_3) + strlen(path6to3_6);
            for (i = 0; i < strlen(path6to3_3); i++)
            {
                path[i] = TB3node[(int)path6to3_3[i] - 48];
                pathlength++;
            }
            path[i] = { 25,10 };
            path[i + 1] = { 31,13 };
            path[i + 2] = { 32,16 };
            path[i + 3] = { 32,18 };
            //path[i + 3] = { 27,10 };
            pathlength += 4;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(path6to3_6); i++, j++, pathlength++)
            {
                path[i] = TB6node[(int)path6to3_6[j] - 48];
            }
            pathlength = length + 5;
            api.Print(fmt::format("pathlength:{}", pathlength));
            path[pathlength - 1] = { destX,destY };/**/
        }
        else if (origArea == 6 && destArea == 3)//六教到三教
        {
            const char* path6to3_6 = PathInAnArea(api, origX, origY, 32, 18, 6);
            const char* path6to3_3 = PathInAnArea(api, 25, 10, destX, destY, 3);
            int i = 0, j = 0;
            int length = strlen(path6to3_3) + strlen(path6to3_6);
            for (i = 0; i < strlen(path6to3_6); i++)
            {
                path[i] = TB6node[(int)path6to3_6[i] - 48];
                pathlength++;
            }
            path[i] = { 32,18 };
            //path[i+1] = { 32,16 };
            path[i + 1] = { 31,13 };
            path[i + 2] = { 27,10 };
            path[i + 3] = { 25,10 };
            pathlength += 4;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(path6to3_3); i++, j++, pathlength++)
            {
                path[i] = TB3node[(int)path6to3_3[j] - 48];
            }
            pathlength = length + 5;
            api.Print(fmt::format("pathlength:{}", pathlength));
            path[pathlength - 1] = { destX,destY };/**/
        }
        else if (origArea == 5 && destArea == 6)//五教到六教
        {
            const char* path5to6_5 = PathInAnArea(api, origX, origY, 17, 31, 5);
            const char* path5to6_6 = PathInAnArea(api, 27, 34, destX, destY, 6);
            int i = 0, j = 0;
            int length = strlen(path5to6_5) + strlen(path5to6_6);
            for (i = 0; i < strlen(path5to6_5); i++)
            {
                path[i] = TB5node[(int)path5to6_5[i] - 48];
                pathlength++;
            }

            path[i] = { 17,31 };
            path[i + 1] = { 17,29 };
            path[i + 2] = { 17,25 };
            path[i + 3] = { 22,25 };
            path[i + 4] = { 25,34 };
            pathlength += 5;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(path5to6_6); i++, j++, pathlength++)
            {
                path[i] = TB6node[(int)path5to6_6[j] - 48];
            }
            pathlength = length + 6;
            path[pathlength - 1] = { destX,destY };
        }
        else if (origArea == 6 && destArea == 5)//六教到五教
        {
            const char* path6to5_6 = PathInAnArea(api, origX, origY, 27, 34, 6);
            const char* path6to5_5 = PathInAnArea(api, 17, 31, destX, destY, 5);
            int i = 0, j = 0;
            int length = strlen(path6to5_5) + strlen(path6to5_6);
            for (i = 0; i < strlen(path6to5_6); i++)
            {
                path[i] = TB6node[(int)path6to5_6[i] - 48];
                pathlength++;
            }

            path[i] = { 25,34 };
            path[i + 1] = { 22,25 };
            path[i + 2] = { 17,25 };
            path[i + 3] = { 17,29 };
            path[i + 4] = { 17,31 };
            pathlength += 5;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(path6to5_5); i++, j++, pathlength++)
            {
                path[i] = TB5node[(int)path6to5_5[j] - 48];
            }
            pathlength = length + 6;
            path[pathlength - 1] = { destX,destY };
        }

        else if (origArea == 1)//如果起点是一教
        {
            MapNode* OrigArea = TB1node, * DestArea;
            WindowNode* DestWindow;
            switch (destArea)//选择合适的教室节点图
            {
            case 3:
            {DestWindow = TB3window;
            DestArea = TB3node;
            break; }
            case 5:
            {DestWindow = TB5window;
            DestArea = TB5node;
            break; }
            case 6:
            {DestWindow = TB6window;
            DestArea = TB6node;
            break; }
            case 1:
            {DestWindow = TB1window;
            DestArea = TB1node;
            break; }
            default:
            {DestWindow = { 0 };
            DestArea = { 0 };
            break; }
            }
            int windowNum = MostCloseWindow(origX, origY, destArea);
            const char* path2window = PathInAnArea(api, origX, origY, DestWindow[windowNum].x_out, DestWindow[windowNum].y_out, origArea);
            const char* pathwindow2dest = PathInAnArea(api, DestWindow[windowNum].x_in, DestWindow[windowNum].y_in, destX, destY, destArea);
            int i = 0, j = 0;
            int length = strlen(path2window) + strlen(pathwindow2dest);
            for (i = 0; i < strlen(path2window); i++)
            {
                path[i] = OrigArea[(int)path2window[i] - 48];
                pathlength++;
            }
            path[i] = { DestWindow[windowNum].x_out,DestWindow[windowNum].y_out };
            path[i + 1] = { DestWindow[windowNum].x_in,DestWindow[windowNum].y_in };
            pathlength += 2;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(pathwindow2dest); i++, j++, pathlength++)
            {
                path[i] = DestArea[(int)pathwindow2dest[j] - 48];
            }
            pathlength = length + 3;
            path[pathlength - 1] = { destX,destY };
        }
        else if (destArea == 1)//终点是一教
        {
            MapNode* OrigArea, * DestArea;
            WindowNode* OrigWindow;
            switch (origArea)//选择合适的教室节点图
            {
            case 3:
            {OrigWindow = TB3window;
            OrigArea = TB3node;
            break; }
            case 5:
            {OrigWindow = TB5window;
            OrigArea = TB5node;
            break; }
            case 6:
            {OrigWindow = TB6window;
            OrigArea = TB6node;
            break; }
            case 1:
            {OrigWindow = TB1window;
            OrigArea = TB1node;
            break; }
            default:
            {OrigWindow = { 0 };
            OrigArea = { 0 };
            break; }
            }
            switch (destArea)//选择合适的教室节点图
            {
            case 3:
            {DestArea = TB3node;
            break; }
            case 5:
            {DestArea = TB5node;
            break; }
            case 6:
            {DestArea = TB6node;
            break; }
            case 1:
            {DestArea = TB1node;
            break; }
            default:
            {DestArea = { 0 };
            break; }
            }
            int windowNum = MostCloseWindow(destX, destY, origArea);
            const char* path2window = PathInAnArea(api, origX, origY, OrigWindow[windowNum].x_in, OrigWindow[windowNum].y_in, origArea);
            const char* pathwindow2dest = PathInAnArea(api, OrigWindow[windowNum].x_out, OrigWindow[windowNum].y_out, destX, destY, destArea);
            int i = 0, j = 0;
            int length = strlen(path2window) + strlen(pathwindow2dest);
            for (i = 0; i < strlen(path2window); i++)
            {
                path[i] = OrigArea[(int)path2window[i] - 48];
                pathlength++;
            }
            path[i] = { OrigWindow[windowNum].x_in,OrigWindow[windowNum].y_in };
            path[i + 1] = { OrigWindow[windowNum].x_out,OrigWindow[windowNum].y_out };
            pathlength += 2;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(pathwindow2dest); i++, j++, pathlength++)
            {
                path[i] = DestArea[(int)pathwindow2dest[j] - 48];
            }
            pathlength = length + 3;
            path[pathlength - 1] = { destX,destY };
        }
        else
            api.Print("no path between orig and dest");
    }
    api.Print("Path has been updated!");
    for (int i = 0; i < pathlength; i++)//便于debug，在每次move的时候打印出路径
    {
        api.Print(fmt::format("path[{}]:{},{}",i, path[i].x, path[i].y));
    }
    FinalDestination = { destX,destY };
    api.Print(fmt::format("Destination:{},{}", destX, destY));
    Arrived = false;
    pathnum = 0;//可能会在之后带来问题
}
void SetPath2(IAPI& api, int origX, int origY, int destX, int destY)
{
    int origArea = 1;
    int destArea = 1;
    if (origArea == destArea)
    {
        const char* path1 = PathInAnArea2(api, origX, origY, destX, destY, origArea);
        MapNode* areanode;
        switch (origArea)//选择合适的教室节点图
        {
        case 3:
        {areanode = TB3node;
        break; }
        case 5:
        {areanode = TB5node;
        break; }
        case 6:
        {areanode = TB6node;
        break; }
        case 1:
        {areanode = TB1node2;
        break; }
        default:
        {areanode = { 0 };
        break; }
        }
        int i = 0;
        for (i = 0; i < strlen(path1); i++)//把path1（字符串）中的路径转为path（结点数组）的元素
        {
            path[i] = areanode[(int)path1[i] - 48];
            pathlength++;
        }
        path[i] = { destX,destY };//把真实要去的点作为最后一个节点
        pathlength++;

    }
    else//如果不在一间教室的情况
    {
        if (origArea == 3 && destArea == 5)//三教到五教
        {
            const char* path3to5_3 = PathInAnArea(api, origX, origY, 17, 22, 3);
            const char* path3to5_5 = PathInAnArea(api, 17, 31, destX, destY, 5);
            api.Print(path3to5_3);
            api.Print(path3to5_5);
            int i = 0, j = 0;
            int length = strlen(path3to5_3) + strlen(path3to5_5);
            for (i = 0; i < strlen(path3to5_3); i++)
            {
                path[i] = TB3node[(int)path3to5_3[i] - 48];
                pathlength++;
            }
            path[i] = { 17,22 };
            path[i + 1] = { 19,25 };
            path[i + 2] = { 17,25 };
            path[i + 3] = { 17,29 };
            pathlength += 4;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(path3to5_5); i++, j++, pathlength++)
            {
                path[i] = TB5node[(int)path3to5_5[j] - 48];
            }
            pathlength = length + 5;
            path[pathlength - 1] = { destX,destY };

        }
        else if (origArea == 5 && destArea == 3)//五教到三教
        {
            const char* path5to3_5 = PathInAnArea(api, origX, origY, 17, 31, 5);
            const char* path5to3_3 = PathInAnArea(api, 17, 22, destX, destY, 3);
            int i = 0, j = 0;
            int length = strlen(path5to3_3) + strlen(path5to3_5);
            for (i = 0; i < strlen(path5to3_5); i++)
            {
                path[i] = TB5node[(int)path5to3_5[i] - 48];
                pathlength++;
            }
            path[i] = { 17,31 };
            path[i + 1] = { 17,29 };
            path[i + 2] = { 17,25 };
            path[i + 3] = { 19,25 };
            path[i + 4] = { 19,22 };
            path[i + 5] = { 17,22 };
            pathlength += 6;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(path5to3_3); i++, j++, pathlength++)
            {
                path[i] = TB3node[(int)path5to3_3[j] - 48];
            }
            pathlength = length + 7;
            path[pathlength - 1] = { destX,destY };

        }
        else if (origArea == 3 && destArea == 6)//三教到六教
        {
            const char* path6to3_3 = PathInAnArea(api, origX, origY, 25, 10, 3);
            const char* path6to3_6 = PathInAnArea(api, 32, 18, destX, destY, 6);
            int i = 0, j = 0;
            int length = strlen(path6to3_3) + strlen(path6to3_6);
            for (i = 0; i < strlen(path6to3_3); i++)
            {
                path[i] = TB3node[(int)path6to3_3[i] - 48];
                pathlength++;
            }
            path[i] = { 25,10 };
            path[i + 1] = { 31,13 };
            path[i + 2] = { 32,16 };
            path[i + 3] = { 32,18 };
            //path[i + 3] = { 27,10 };
            pathlength += 4;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(path6to3_6); i++, j++, pathlength++)
            {
                path[i] = TB6node[(int)path6to3_6[j] - 48];
            }
            pathlength = length + 5;
            api.Print(fmt::format("pathlength:{}", pathlength));
            path[pathlength - 1] = { destX,destY };/**/
        }
        else if (origArea == 6 && destArea == 3)//六教到三教
        {
            const char* path6to3_6 = PathInAnArea(api, origX, origY, 32, 18, 6);
            const char* path6to3_3 = PathInAnArea(api, 25, 10, destX, destY, 3);
            int i = 0, j = 0;
            int length = strlen(path6to3_3) + strlen(path6to3_6);
            for (i = 0; i < strlen(path6to3_6); i++)
            {
                path[i] = TB6node[(int)path6to3_6[i] - 48];
                pathlength++;
            }
            path[i] = { 32,18 };
            //path[i+1] = { 32,16 };
            path[i + 1] = { 31,13 };
            path[i + 2] = { 27,10 };
            path[i + 3] = { 25,10 };
            pathlength += 4;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(path6to3_3); i++, j++, pathlength++)
            {
                path[i] = TB3node[(int)path6to3_3[j] - 48];
            }
            pathlength = length + 5;
            api.Print(fmt::format("pathlength:{}", pathlength));
            path[pathlength - 1] = { destX,destY };/**/
        }
        else if (origArea == 5 && destArea == 6)//五教到六教
        {
            const char* path5to6_5 = PathInAnArea(api, origX, origY, 17, 31, 5);
            const char* path5to6_6 = PathInAnArea(api, 27, 34, destX, destY, 6);
            int i = 0, j = 0;
            int length = strlen(path5to6_5) + strlen(path5to6_6);
            for (i = 0; i < strlen(path5to6_5); i++)
            {
                path[i] = TB5node[(int)path5to6_5[i] - 48];
                pathlength++;
            }

            path[i] = { 17,31 };
            path[i + 1] = { 17,29 };
            path[i + 2] = { 17,25 };
            path[i + 3] = { 22,25 };
            path[i + 4] = { 25,34 };
            pathlength += 5;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(path5to6_6); i++, j++, pathlength++)
            {
                path[i] = TB6node[(int)path5to6_6[j] - 48];
            }
            pathlength = length + 6;
            path[pathlength - 1] = { destX,destY };
        }
        else if (origArea == 6 && destArea == 5)//六教到五教
        {
            const char* path6to5_6 = PathInAnArea(api, origX, origY, 27, 34, 6);
            const char* path6to5_5 = PathInAnArea(api, 17, 31, destX, destY, 5);
            int i = 0, j = 0;
            int length = strlen(path6to5_5) + strlen(path6to5_6);
            for (i = 0; i < strlen(path6to5_6); i++)
            {
                path[i] = TB6node[(int)path6to5_6[i] - 48];
                pathlength++;
            }

            path[i] = { 25,34 };
            path[i + 1] = { 22,25 };
            path[i + 2] = { 17,25 };
            path[i + 3] = { 17,29 };
            path[i + 4] = { 17,31 };
            pathlength += 5;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(path6to5_5); i++, j++, pathlength++)
            {
                path[i] = TB5node[(int)path6to5_5[j] - 48];
            }
            pathlength = length + 6;
            path[pathlength - 1] = { destX,destY };
        }

        else if (origArea == 1)//如果起点是一教
        {
            MapNode* OrigArea = TB1node, * DestArea;
            WindowNode* DestWindow;
            switch (destArea)//选择合适的教室节点图
            {
            case 3:
            {DestWindow = TB3window;
            DestArea = TB3node;
            break; }
            case 5:
            {DestWindow = TB5window;
            DestArea = TB5node;
            break; }
            case 6:
            {DestWindow = TB6window;
            DestArea = TB6node;
            break; }
            case 1:
            {DestWindow = TB1window;
            DestArea = TB1node;
            break; }
            default:
            {DestWindow = { 0 };
            DestArea = { 0 };
            break; }
            }
            int windowNum = MostCloseWindow(origX, origY, destArea);
            const char* path2window = PathInAnArea(api, origX, origY, DestWindow[windowNum].x_out, DestWindow[windowNum].y_out, origArea);
            const char* pathwindow2dest = PathInAnArea(api, DestWindow[windowNum].x_in, DestWindow[windowNum].y_in, destX, destY, destArea);
            int i = 0, j = 0;
            int length = strlen(path2window) + strlen(pathwindow2dest);
            for (i = 0; i < strlen(path2window); i++)
            {
                path[i] = OrigArea[(int)path2window[i] - 48];
                pathlength++;
            }
            path[i] = { DestWindow[windowNum].x_out,DestWindow[windowNum].y_out };
            path[i + 1] = { DestWindow[windowNum].x_in,DestWindow[windowNum].y_in };
            pathlength += 2;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(pathwindow2dest); i++, j++, pathlength++)
            {
                path[i] = DestArea[(int)pathwindow2dest[j] - 48];
            }
            pathlength = length + 3;
            path[pathlength - 1] = { destX,destY };
        }
        else if (destArea == 1)//终点是一教
        {
            MapNode* OrigArea, * DestArea;
            WindowNode* OrigWindow;
            switch (origArea)//选择合适的教室节点图
            {
            case 3:
            {OrigWindow = TB3window;
            OrigArea = TB3node;
            break; }
            case 5:
            {OrigWindow = TB5window;
            OrigArea = TB5node;
            break; }
            case 6:
            {OrigWindow = TB6window;
            OrigArea = TB6node;
            break; }
            case 1:
            {OrigWindow = TB1window;
            OrigArea = TB1node;
            break; }
            default:
            {OrigWindow = { 0 };
            OrigArea = { 0 };
            break; }
            }
            switch (destArea)//选择合适的教室节点图
            {
            case 3:
            {DestArea = TB3node;
            break; }
            case 5:
            {DestArea = TB5node;
            break; }
            case 6:
            {DestArea = TB6node;
            break; }
            case 1:
            {DestArea = TB1node;
            break; }
            default:
            {DestArea = { 0 };
            break; }
            }
            int windowNum = MostCloseWindow(destX, destY, origArea);
            const char* path2window = PathInAnArea(api, origX, origY, OrigWindow[windowNum].x_in, OrigWindow[windowNum].y_in, origArea);
            const char* pathwindow2dest = PathInAnArea(api, OrigWindow[windowNum].x_out, OrigWindow[windowNum].y_out, destX, destY, destArea);
            int i = 0, j = 0;
            int length = strlen(path2window) + strlen(pathwindow2dest);
            for (i = 0; i < strlen(path2window); i++)
            {
                path[i] = OrigArea[(int)path2window[i] - 48];
                pathlength++;
            }
            path[i] = { OrigWindow[windowNum].x_in,OrigWindow[windowNum].y_in };
            path[i + 1] = { OrigWindow[windowNum].x_out,OrigWindow[windowNum].y_out };
            pathlength += 2;
            int bug = pathlength;
            for (i = bug; i < bug + strlen(pathwindow2dest); i++, j++, pathlength++)
            {
                path[i] = DestArea[(int)pathwindow2dest[j] - 48];
            }
            pathlength = length + 3;
            path[pathlength - 1] = { destX,destY };
        }
        else
            api.Print("no path between orig and dest");
    }
    api.Print("Path has been updated!");
    for (int i = 0; i < pathlength; i++)//便于debug，在每次move的时候打印出路径
    {
        api.Print(fmt::format("path[{}]:{},{}", i, path[i].x, path[i].y));
    }
    FinalDestination = { destX,destY };
    api.Print(fmt::format("Destination:{},{}", destX, destY));
    Arrived = false;
    pathnum = 0;//可能会在之后带来问题
}
bool MoveAlongPath(IStudentAPI& api)//当到达目的地时返回true
{
    auto self = api.GetSelfInfo();
    if (!Arrived)//测试用片段，沿着path中的结点前进
    {
        int nowCount = api.GetFrameCount();//这一部分用来实现翻墙
        api.Print(fmt::format("{},{}", nowCount, lastSkipCount));
        if (BeStuck(api))
        {
            int selfx = self->x;
            int selfy = self->y;
            auto delta_x = path[pathnum].x - selfx;
            auto delta_y = path[pathnum].y - selfy;
            double ang = 0;
            ang = atan2(delta_y, delta_x);
            if (Stuck)
            {
                ang += PI / 2.0;
                Stuck = false;
            }
            else
            {
                ang -= PI / 2.0;
                Stuck = true;
            }
            api.Move(80, ang);
        }
        if (nowCount - lastSkipCount > 100)
        {
            if (api.SkipWindow().get())
            {
                pathnum++;
                Goto(api, path[pathnum].x, path[pathnum].y);
                lastSkipCount = nowCount;
            }
        }
        if (map[path[pathnum].x][path[pathnum].y] == THUAI6::PlaceType::ClassRoom || map[path[pathnum].x][path[pathnum].y] == THUAI6::PlaceType::Gate)
        {
            if (!arriveAtMax(api, path[pathnum].x, path[pathnum].y))
            {
                api.Print(fmt::format("TempDestination:{},{}", path[pathnum].x, path[pathnum].y));
                Goto(api, path[pathnum].x, path[pathnum].y);
                return false;
            }
            else
            {
                if (pathnum < pathlength - 1)//如果到达了一个节点且路还没走完
                {
                    pathnum++;
                    return false;
                }
                else//如果到达的是最后一个节点
                {
                    api.Print("到达了最后一个节点");
                    Arrived = true;
                    return true;
                }
            }
        }
        else
        {
            if (!arriveAt(api, path[pathnum].x, path[pathnum].y))
            {
                api.Print(fmt::format("TempDestination:{},{}", path[pathnum].x, path[pathnum].y));
                Goto(api, path[pathnum].x, path[pathnum].y);
                return false;
            }
            else
            {
                if (pathnum < pathlength - 1)//如果到达了一个节点且路还没走完
                {
                    pathnum++;
                    return false;
                }
                else//如果到达的是最后一个节点
                {
                    api.Print("到达了最后一个节点");
                    pathnum = 0;
                    pathlength = 0;
                    Arrived = true;
                    return true;
                    memset(path, 0, 100);
                }
            }
        }
    }
    else
        return true;
}
bool MoveAlongPath(ITrickerAPI& api)//当到达目的地时返回true
{
    auto self = api.GetSelfInfo();
    if (!Arrived)//测试用片段，沿着path中的结点前进
    {
        int nowCount = api.GetFrameCount();//这一部分用来实现翻墙
        api.Print(fmt::format("{},{}", nowCount, lastSkipCount));
        if (BeStuck(api))
        {
            int selfx = self->x;
            int selfy = self->y;
            auto delta_x = path[pathnum].x - selfx;
            auto delta_y = path[pathnum].y - selfy;
            double ang = 0;
            ang = atan2(delta_y, delta_x);
            if (Stuck)
            {
                ang += PI / 2.0;
                Stuck = false;
            }
            else
            {
                ang -= PI / 2.0;
                Stuck = true;
            }
            api.Move(80, ang);
        }

        if (nowCount - lastSkipCount > 100)
        {
            if (api.SkipWindow().get())
            {
                pathnum++;
                Goto(api, path[pathnum].x, path[pathnum].y);
                lastSkipCount = nowCount;
            }
        }
        if (!arriveAt(api, path[pathnum].x, path[pathnum].y))
        {
            api.Print(fmt::format("TempDestination:{},{}", path[pathnum].x, path[pathnum].y));
            Goto(api, path[pathnum].x, path[pathnum].y);
            return false;
        }
        else
        {
            if (pathnum < pathlength - 1)//如果到达了一个节点且路还没走完
            {
                pathnum++;
                Goto(api, path[pathnum].x, path[pathnum].y);
                return false;
            }
            else//如果到达的是最后一个节点
            {
                api.Print("到达了最后一个节点");
                pathnum = 0;
                pathlength = 0;
                Arrived = true;
                return true;
                memset(path, 0, 100);
            }
        }
    }
    else
        return true;
}
bool MoveAlongClosedPath(ITrickerAPI& api)//当到达目的地时返回true
{
    auto self = api.GetSelfInfo();
    if (!Arrived)//测试用片段，沿着Closedpath中的结点前进
    {
        int nowCount = api.GetFrameCount();//这一部分用来实现翻墙
        api.Print(fmt::format("{},{}", nowCount, lastSkipCount));
        if (nowCount - lastSkipCount > 100)
        {
            if (api.SkipWindow().get())
            {
                pathnum++;
                Goto(api, path[pathnum].x, path[pathnum].y);
                lastSkipCount = nowCount;
            }
        }
        if (BeStuck(api))
        {
            int selfx = self->x;
            int selfy = self->y;
            auto delta_x = ClosedTrickerPath[pathnum].x - selfx;
            auto delta_y = ClosedTrickerPath[pathnum].y - selfy;
            double ang = 0;
            ang = atan2(delta_y, delta_x);
            if (Stuck)
            {
                ang += PI / 2.0;
                Stuck = false;
            }
            else
            {
                ang -= PI / 2.0;
                Stuck = true;
            }
            api.Move(80, ang);
        }
        
        if (!arriveAt(api, ClosedTrickerPath[pathnum].x, ClosedTrickerPath[pathnum].y))
        {
            api.Print(fmt::format("TempDestination:{},{}", ClosedTrickerPath[pathnum].x, ClosedTrickerPath[pathnum].y));
            Goto(api, ClosedTrickerPath[pathnum].x, ClosedTrickerPath[pathnum].y);
            return false;
        }
        else
        {
            if (pathnum < Closedpathlength - 1)//如果到达了一个节点且路还没走完
            {
                pathnum++;
                Goto(api, ClosedTrickerPath[pathnum].x, ClosedTrickerPath[pathnum].y);
                return false;
            }
            else//如果到达的是最后一个节点
            {
                pathnum = 0;
                Goto(api, ClosedTrickerPath[pathnum].x, ClosedTrickerPath[pathnum].y);
                return false;
            }
        }
    }
    else
        return true;
}
bool MoveAlongClosedPath2(ITrickerAPI& api)//当到达目的地时返回true
{
    auto self = api.GetSelfInfo();
    if (!Arrived)//测试用片段，沿着Closedpath中的结点前进
    {
        int nowCount = api.GetFrameCount();//这一部分用来实现翻墙
        api.Print(fmt::format("{},{}", nowCount, lastSkipCount));
        if (nowCount - lastSkipCount > 100)
        {
            if (api.SkipWindow().get())
            {
                pathnum++;
                Goto(api, path[pathnum].x, path[pathnum].y);
                lastSkipCount = nowCount;
            }
        }
        if (BeStuck(api))
        {
            int selfx = self->x;
            int selfy = self->y;
            auto delta_x = ClosedTrickerPath2[pathnum].x - selfx;
            auto delta_y = ClosedTrickerPath2[pathnum].y - selfy;
            double ang = 0;
            ang = atan2(delta_y, delta_x);
            if (Stuck)
            {
                ang += PI / 2.0;
                Stuck = false;
            }
            else
            {
                ang -= PI / 2.0;
                Stuck = true;
            }
            api.Move(80, ang);
        }

        if (!arriveAt(api, ClosedTrickerPath2[pathnum].x, ClosedTrickerPath2[pathnum].y))
        {
            api.Print(fmt::format("TempDestination:{},{}", ClosedTrickerPath2[pathnum].x, ClosedTrickerPath2[pathnum].y));
            Goto(api, ClosedTrickerPath2[pathnum].x, ClosedTrickerPath2[pathnum].y);
            return false;
        }
        else
        {
            if (pathnum < Closedpathlength2 - 1)//如果到达了一个节点且路还没走完
            {
                pathnum++;
                Goto(api, ClosedTrickerPath2[pathnum].x, ClosedTrickerPath2[pathnum].y);
                return false;
            }
            else//如果到达的是最后一个节点
            {
                pathnum = 0;
                Goto(api, ClosedTrickerPath2[pathnum].x, ClosedTrickerPath2[pathnum].y);
                return false;
            }
        }
    }
    else
        return true;
}
bool MoveAlongSettedPath(IStudentAPI& api)//当到达目的地时返回true
{
    auto self = api.GetSelfInfo();
    int id = self->playerID;
    if (!Arrived1)//测试用片段，沿着path中的结点前进
    {
        int nowCount = api.GetFrameCount();//这一部分用来实现翻墙
        api.Print(fmt::format("{},{}", nowCount, lastSkipCount));
        if (BeStuck(api))
        {
            int selfx = self->x;
            int selfy = self->y;
            auto delta_x = (Ppath[id] + pathnum)->x - GridToCell(selfx);
            auto delta_y = (Ppath[id] + pathnum)->y - GridToCell(selfy);
            double ang = 0;
            ang = atan2(delta_y, delta_x);
            if (Stuck)
            {
                ang += PI / 2.0;
                Stuck = false;
            }
            else
            {
                ang -= PI / 2.0;
                Stuck = true;
            }
            api.Move(80, ang);
        }
        if (nowCount - lastSkipCount > 100)
        {
            if (api.SkipWindow().get())
            {
                pathnum++;
                Goto(api, path[pathnum].x, path[pathnum].y);
                lastSkipCount = nowCount;
            }
        }
        
        if (!arriveAt(api, (Ppath[id]+pathnum)->x, (Ppath[id] + pathnum)->y))
        {
            //api.Print(fmt::format("TempDestination:{},{}", path[pathnum].x, path[pathnum].y));
            Goto(api, (Ppath[id] + pathnum)->x, (Ppath[id] + pathnum)->y);
            return false;
        }
        else
        {
            if (pathnum < PathforLength[id]-1)//如果到达了一个节点且路还没走完
            {
                pathnum++;
                return false;
            }
            else//如果到达的是最后一个节点
            {
                api.Print("到达了最后一个节点");
                pathnum = 0;
                pathlength = 0;
                Arrived1 = true;
                return true;
                memset(path, 0, 100);
                api.UseSkill(0);
            }
        }
    }
    else
        return true;
}
bool MoveAlongSettedPath2(IStudentAPI& api)//当到达目的地时返回true
{
    auto self = api.GetSelfInfo();
    int id = self->playerID;
    if (!Arrived1)//测试用片段，沿着path中的结点前进
    {
        int nowCount = api.GetFrameCount();//这一部分用来实现翻墙
        api.Print(fmt::format("{},{}", nowCount, lastSkipCount));
        if (BeStuck(api))
        {
            int selfx = self->x;
            int selfy = self->y;
            auto delta_x = (Ppath2[id] + pathnum)->x - selfx;
            auto delta_y = (Ppath2[id] + pathnum)->y - selfy;
            double ang = 0;
            ang = atan2(delta_y, delta_x);
            if (Stuck)
            {
                ang += PI / 2.0;
                Stuck = false;
            }
            else
            {
                ang -= PI / 2.0;
                Stuck = true;
            }
            api.Move(80, ang);
        }
        if (nowCount - lastSkipCount > 100)
        {
            if (api.SkipWindow().get())//没用
            {
                pathnum++;
                Goto(api, path[pathnum].x, path[pathnum].y);
                lastSkipCount = nowCount;
            }
        }



        if (!arriveAt(api, (Ppath2[id] + pathnum)->x, (Ppath2[id] + pathnum)->y))
        {
            //api.Print(fmt::format("TempDestination:{},{}", path[pathnum].x, path[pathnum].y));
            Goto(api, (Ppath2[id] + pathnum)->x, (Ppath2[id] + pathnum)->y);
            return false;
        }
        else
        {
            if (pathnum < PathforLength2[id] - 1)//如果到达了一个节点且路还没走完
            {
                pathnum++;
                return false;
            }
            else//如果到达的是最后一个节点
            {
                api.Print("到达了最后一个节点");
                pathnum = 0;
                pathlength = 0;
                Arrived1 = true;
                return true;
                memset(path, 0, 100);
                api.UseSkill(0);
            }
        }

    }
    else
        return true;
}

int MostCloseWindow(int x, int y, int AreaNum)//返回最近的窗子的数组下标索引,不考虑墙的遮挡
{
    WindowNode* areanode;
    int Windownum, result = -1;
    switch (AreaNum)//选择合适的教室节点图
    {
    case 3:
    {areanode = TB3window;
    Windownum = TB3windowNum;
    break; }
    case 5:
    {areanode = TB5window;
    Windownum = TB5windowNum;
    break; }
    case 6:
    {areanode = TB6window;
    Windownum = TB6windowNum;
    break; }
    case 1:
    {areanode = TB1window;
    Windownum = TB1windowNum;
    break; }
    default:
    {areanode = { 0 };
    Windownum = 0;
    break; }
    }
    double dis, min_dis = 100;
    for (int i = 0; i < Windownum; i++)
    {
        double x_p = areanode[i].x_out;
        double y_p = areanode[i].y_out;
        dis = sqrt((x - x_p) * (x - x_p) + (y - y_p) * (y - y_p));
        if (dis < min_dis)
        {
            min_dis = dis;
            result = i;
        }
    }
    return result;
}
Classroom *ChooseAClassroom(IStudentAPI& api, int x, int y,int ID)//寻找未写完的最近的作业(先在同一个区域找，若没有，在所有区域找）
{
    Classroom *dest=&TB3[1];
    int area = GetArea(x, y);   //获取现在所处位置所在的区域
    int flag = 1;
    double dis, min_dis = 100;
    switch (area)
    {
    case 3:
    
    {for (int i = 0; i < 3; ++i)       //看三教中所有的作业
    {
        //api.Print(fmt::format("progress{}", TB3[i].progress));
        if (TB3[i].progress != Constants::maxClassroomProgress)  //如果有作业没写完
        {
            //api.Print("into the if");
            dis = GetDistance(x, y, TB3[i].xy[ID].x, TB3[i].xy[ID].y);
            if (dis < min_dis)
            {
                min_dis = dis;
                dest = &TB3[i];
            }
            flag = 0;
        }
    }
    break;
    }
    case 5:
    
    {for (int i = 0; i < 3; ++i)       //看五教中所有的作业
        {
            if (TB5[i].progress != Constants::maxClassroomProgress)  //如果有作业没写完
            {
                dis = GetDistance(x, y, TB5[i].xy[ID].x, TB5[i].xy[ID].y);
                if (dis < min_dis)
                {
                    min_dis = dis;
                    dest = &TB5[i];
                }
                flag = 0;
            }
        }
        break; }
    case 6:
        //api.Print("finding in TB6");
    {for (int i = 0; i < 4; ++i)       //看六教中所有的作业
    {
        if (TB6[i].progress != Constants::maxClassroomProgress)  //如果有作业没写完
        {
            dis = GetDistance(x, y, TB6[i].xy[ID].x, TB6[i].xy[ID].y);
            if (dis < min_dis)
            {
                min_dis = dis;
                dest = &TB6[i];
            }
            flag = 0;
        }
    }
    break; }
    case 1:
        //api.Print("finding in TB1");
    {for (int i = 0; i < 3; ++i)       //看三教中所有的作业
    {
        if (TB3[i].progress != Constants::maxClassroomProgress)  //如果有作业没写完
        {
            dis = GetDistance(x, y, TB3[i].xy[ID].x, TB3[i].xy[ID].y);
            if (dis < min_dis)
            {
                min_dis = dis;
                dest = &TB3[i];
            }
            flag = 0;
        }
    }
    for (int i = 0; i < 3; ++i)       //看五教中所有的作业
    {
        if (TB5[i].progress != Constants::maxClassroomProgress)  //如果有作业没写完
        {
            dis = GetDistance(x, y, TB5[i].xy[ID].x, TB5[i].xy[ID].y);
            if (dis < min_dis)
            {
                min_dis = dis;
                dest = &TB5[i];
            }
            flag = 0;
        }
    }
    for (int i = 0; i < 4; ++i)       //看六教中所有的作业
    {
        if (TB6[i].progress != Constants::maxClassroomProgress)  //如果有作业没写完
        {
            dis = GetDistance(x, y, TB6[i].xy[ID].x, TB6[i].xy[ID].y);
            if (dis < min_dis)
            {
                min_dis = dis;
                dest = &TB6[i];
            }
            flag = 0;
        }
    }
    break; }
    default:
        break;
    }           //switch结构完成
    if (flag)   //flag仍为1，即没找到所在区域未完成的作业，或作业已全部被完成
    {
        //api.Print("在自己的区域没找到");
        for (int i = 0; i < 3; ++i)       //看三教中所有的作业
        {
            if (TB3[i].progress != Constants::maxClassroomProgress)  //如果有作业没写完
            {
                dis = GetDistance(x, y, TB3[i].xy[ID].x, TB3[i].xy[ID].y);
                if (dis < min_dis)
                {
                    min_dis = dis;
                    dest = &TB3[i];
                }
                flag = 0;
            }
        }
        for (int i = 0; i < 3; ++i)       //看五教中所有的作业
        {
            if (TB5[i].progress != Constants::maxClassroomProgress)  //如果有作业没写完
            {
                dis = GetDistance(x, y, TB5[i].xy[ID].x, TB5[i].xy[ID].y);
                if (dis < min_dis)
                {
                    min_dis = dis;
                    dest = &TB5[i];
                }
                flag = 0;
            }
        }
        for (int i = 0; i < 4; ++i)       //看六教中所有的作业
        {
            if (TB6[i].progress != Constants::maxClassroomProgress)  //如果有作业没写完
            {
                dis = GetDistance(x, y, TB6[i].xy[ID].x, TB6[i].xy[ID].y);
                if (dis < min_dis)
                {
                    min_dis = dis;
                    dest = &TB6[i];
                }
                flag = 0;
            }
        }//再全看一遍，确保若flag仍为1则全部作业已被完成
    }
    return dest;
}
Gate *ChooseAGate(IStudentAPI& api, int x, int y)
{
    for (int i = 2; i < 6; i++)//先刷新所有校门的状态
    {
        gate[i].Status = (int)api.GetHiddenGateState(gate[i].xy.x, gate[i].xy.y);
        api.Print(fmt::format("gate[{}]status:{}", i, gate[i].Status));
    }
    double dis, min_dis = 100;
    int result = 0;
    for (int i = 0; i < 6; i++)
    {
        double x_p = gate[i].xy.x;
        double y_p = gate[i].xy.y;
        dis = sqrt((x - x_p) * (x - x_p) + (y - y_p) * (y - y_p));
        if (dis < min_dis&&gate[i].Status==1)
        {
            min_dis = dis;
            result = i;
        }
    }
    api.Print(fmt::format("Gate[{}]", result));
    return &gate[result];
}
int ChooseAnArea(ITrickerAPI& api)
{
    //暂时根据离得最近的窗子去选择，后续要调整，优化算法
    auto self = api.GetSelfInfo();
    int result,windowNum;
    windowNum = MostCloseWindow(GridToCell(self->x), GridToCell(self->y),1);
    if (windowNum <= 3)
        result = 3;
    else if (windowNum >= 7)
        result = 6;
    else
        result = 3;//先不去五教
    return result;
}

void InitMap(IAPI& api)
{
    int i, j;
    auto mapinfo = api.GetFullMap();
    for (i = 0; i < 50; i++)
        for (j = 0; j < 50; j++)
        {
            map[i][j] = mapinfo[i][j];
        }
    api.Wait();
}
void SetStatus(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    auto students = api.GetStudents();
    auto GameInfo = api.GetGameInfo();
    auto bulletType = self->bulletType;
    api.Print(fmt::format("BulletType:{}", THUAI6::bulletTypeDict[bulletType]));
    api.Print(fmt::format("StudentNum:{}", students.size()));
    if (BeLongStuck(api)&&Tricker1Status==TrickerStatus::Searching)
    {
        StatusIsSearching = false;
    }
    
        if (MostCloseStudent(api) != -1)
        {
            Tricker1Status = TrickerStatus::Tracing;
            StatusIsSearching = false;//说明这次setstatus现在的状态不是searching
            int closednum = MostCloseStudent(api);
            ClosedStudent = { students[closednum]->x, students[closednum]->y };
            if (GetDistance(self->x, self->y, students[closednum]->x, students[closednum]->y) < AttackRange[(int)self->bulletType] - 1000)
            {
                Tricker1Status = TrickerStatus::Attaking;
            }

        }
        else if (students.size()==0&&self->trickDesire>16)
        {
            api.Print("Special sign");
                Tricker1Status = TrickerStatus::Attaking;
        }
        else
        {
            if (FinishedClassroom > 7)
            {
                Tricker1Status = TrickerStatus::GotoGate;
                StatusIsSearching = false;
            }
            Tricker1Status = TrickerStatus::Searching;
        }
    api.Print(fmt::format("Status:{}", (int)Tricker1Status));
}
void SetStatus(IStudentAPI& api)
{
    
}
void UpdateInfo(IStudentAPI& api)
{
    auto self = api.GetSelfInfo();
    auto tricker = api.GetTrickers();
    auto students = api.GetStudents();
    //更新作业状态
    FinishedClassroom = 0;
    for (int i = 0; i < 3; i++)
    {
        if (TB3[i].progress == 10000000)
            FinishedClassroom += 1;
            
    }
    for (int i = 0; i < 3; i++)
    {
        if (TB5[i].progress == 10000000)
            FinishedClassroom += 1;
    }
    for (int i = 0; i < 4; i++)
    {
        if (TB6[i].progress == 10000000)
            FinishedClassroom += 1;
    }
    //更新宝箱状态
    
}
void UpdateInfo(ITrickerAPI& api,int ID)
{
    auto self = api.GetSelfInfo();
    auto Students = api.GetTrickers();
    auto GameInfo = api.GetGameInfo();
    //更新作业状态
    FinishedClassroom = 0;
    for (int i = 0; i < 3; i++)
    {
        TB3[i].progress = api.GetClassroomProgress(TB3[i].xy[ID].x, TB3[i].xy[ID].y);
        if (TB3[i].progress == 10000000)
        {
            TB3FinishedClassroom += 1;
        }

    }
    for (int i = 0; i < 3; i++)
    {
        TB5[i].progress = api.GetClassroomProgress(TB5[i].xy[ID].x, TB5[i].xy[ID].y);
        if (TB5[i].progress == 10000000)
        {
            TB5FinishedClassroom += 1;
        }
    }
    for (int i = 0; i < 4; i++)
    {
        TB6[i].progress = api.GetClassroomProgress(TB6[i].xy[ID].x, TB6[i].xy[ID].y);
        if (TB6[i].progress == 10000000)
        {
            TB6FinishedClassroom += 1;
        }
    }
    FinishedClassroom = GameInfo->subjectFinished;
    //更新宝箱状态

}

void DealingWithMessages(IStudentAPI& api)
{
    if (api.HaveMessage())
    {
        auto mes = api.GetMessage();
        api.Print("GotMessage");
        char c1 = mes.second[0];      //mes类型的第二个参数是一个string类对象,其第一个元素是信息类型       
        if (c1 == '0')    //根据第一个字符来确定类型，'0'类型是关于作业的
        {
            char c2 = mes.second[1];         //string类对象的第二个元素是作业序号
            int num = (int)c2 - 48;          //字符转数字
            int len = mes.second.length();   //字符串的长度
            char* c3 = new char[len - 1];    //减去前两位，再给结束符留一个位置，所以是len-1
            strcpy(c3, mes.second.c_str() + 2);    //不能直接用c3=mes.second.c_str()，否则析构时会被破坏
            long newprogress = atol(c3);                    //转成长整型，不能是整型，否则位数不够
            (*AllClassroom[num]).progress = newprogress;    //只改进度就行，作业坐标不会变
        }
        else if (c1 == '1')
        {

        }
    }
}
int MostCloseStudent(ITrickerAPI &api)
{
    auto self = api.GetSelfInfo();
    auto students = api.GetStudents();
    int num = students.size();
    int result = -1;
    double dis,min_dis=100000;
    for (int i = 0; i < num; i++)
    {
        dis = sqrt((self->x - students[i]->x) * (self->x - students[i]->x) + (self->y - students[i]->y) * (self->y - students[i]->y));
        if (dis < min_dis)
        {
            api.Print(fmt::format("Student[{}]Determination:{}", i, students[i]->determination));
            if (students[i]->determination > 0)
            {
                min_dis = dis;
                result = i;
            }
        }
        api.Print(fmt::format("MostCloseStudent return {}", result));
    }
    return result;
}
bool BeStuck(ITrickerAPI& api)
{
    int nowStayCount = api.GetFrameCount();
    api.Print(fmt::format("{},{}", nowStayCount, lastStayCount));
    auto self = api.GetSelfInfo();
    bool flag = 0;
    MapNode newposition = { GridToCell(self->x),GridToCell(self->y) };
    if (nowStayCount - lastStayCount > 13)
    {
        if (lastposition == newposition && self->playerState == THUAI6::PlayerState::Idle)
        {
            flag = 1;
            api.Print("BeStuck!!!!");
        }
        lastStayCount = nowStayCount;
        lastposition = newposition;
    }
    return flag;
}
bool BeStuck(IStudentAPI & api)
{
    int nowStayCount = api.GetFrameCount();
    api.Print(fmt::format("{},{}", nowStayCount, lastStayCount));
    auto self = api.GetSelfInfo();
    bool flag = 0;
    MapNode newposition = { GridToCell(self->x),GridToCell(self->y) };
    if (nowStayCount - lastStayCount > 13)
    {
        if (lastposition == newposition && self->playerState == THUAI6::PlayerState::Idle)
        {
            flag = 1;
            api.Print("BeStuck!!!!");
        }
        lastStayCount = nowStayCount;
        lastposition = newposition;
    }
    return flag;
}
int lastStayCount1 = 0;
bool BeLongStuck(ITrickerAPI& api)
{
    int nowStayCount = api.GetFrameCount();
    api.Print(fmt::format("{},{}", nowStayCount, lastStayCount1));
    auto self = api.GetSelfInfo();
    bool flag = 0;
    MapNode newposition = { GridToCell(self->x),GridToCell(self->y) };
    if (nowStayCount - lastStayCount1 > 50)
    {
        if (lastposition1 == newposition && self->playerState == THUAI6::PlayerState::Idle)
        {
            flag = 1;
            api.Print("BeLongStuck!!!!");
        }
        lastStayCount1 = nowStayCount;
        lastposition1 = newposition;
    }
    return flag;
}

void UsingSkills(IStudentAPI &api)
{
    auto self = api.GetSelfInfo();
    switch (self->studentType)
    {
    case THUAI6::StudentType::StraightAStudent:
    {
        break;
    }

    };
}//暂时没用到
bool ifarrived = true;//调试用变量，不重要

/*供调试所用代码合集
（1）设置一个path并查看
    InitMap(api);
    SetPath(api, 35, 24, 28, 26);
    for (int i = 0; i < pathnum; i++)
    {
        api.Print(fmt::format("{},{}", path[i].x, path[i].y));
    }
（2）设置一个path并沿着这条路走
    if (ifarrived)
        {
            SetPath(api, GridToCell(self->x), GridToCell(self->y), 4, 14);
            ifarrived = false;
        }
    if(!Arrived)
        MoveAlongPath(api);
（3）
*/
bool test = true;
bool test1 = true;
bool test2 = true;

void AI::play(IStudentAPI& api)
{
    // 公共操作
    if (this->playerID == 0)
    {
        InitMap(api);
        if (map[20][20] == THUAI6::PlaceType::Wall)
        {
            int ID = 0;
            auto self = api.GetSelfInfo();
            auto tricker = api.GetTrickers();
            auto GAME = api.GetGameInfo();
            api.Print(fmt::format("GameTime:{}", GAME->gameTime));

            NowArea = GetArea(GridToCell(self->x), GridToCell(self->y));
            UpdateInfo(api);
            api.Print(fmt::format("cell:{},{}", (Ppath[ID] + PathforLength[ID] - 1)->x, (Ppath[ID] + PathforLength[ID] - 1)->y));
            if (GAME->gameTime < 580000)
            {
                if (test)
                {
                    SetPath(api, GridToCell(self->x), GridToCell(self->y), 46, 7);
                    test = false;
                }
                Stu1Status = StudentStatus::GoToWaitingPoint;
            }
            else
            {
                Stu1Status = StudentStatus::GoToStudy;
            }
            SetStatus(api);//空的
            DealingWithMessages(api);
            UsingSkills(api);//空的
            api.Print(fmt::format("Status:{}", (int)Stu1Status));

            switch (Stu1Status)
            {
            case StudentStatus::Init:
            {InitMap(api);
            Stu1Status = StudentStatus::WantToStudy;
            break; }

            case StudentStatus::WantToStudy:
            {

                TempClassroom = ChooseAClassroom(api, GridToCell(self->x), GridToCell(self->y), 0);
                api.Print(fmt::format("Tempclassroom{}", TempClassroom->id));
                MapNode NewDestination = TempClassroom->xy[this->playerID];
                if (NewDestination != FinalDestination)
                {
                    //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                    pathnum = 0;
                    pathlength = 0;
                    memset(path, 0, 100);

                    SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination.x, NewDestination.y);
                }
                if (MoveAlongPath(api))
                {
                    if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                    {
                        api.UseSkill(0);
                    }
                    Stu1Status = StudentStatus::Studying;
                };
                if (FinishedClassroom >= 7)
                {
                    Stu1Status = StudentStatus::WantToGraduate;
                }
                break; }

            case StudentStatus::Studying:
            {
                api.Print(fmt::format("TempClassroom:{},{},progress{}", TempClassroom->realxy.x, TempClassroom->realxy.y, TempClassroom->progress));
                TempClassroom->progress = api.GetClassroomProgress(TempClassroom->realxy.x, TempClassroom->realxy.y);
                if (TempClassroom->progress < 10000000)
                {
                    api.StartLearning();
                    //api.EndAllAction();
                }
                else
                {
                    for (int i = 0; i < 4; i++)
                    {
                        api.SendTextMessage(i, fmt::format("0{}{}", TempClassroom->id, TempClassroom->progress));
                        api.Print(fmt::format("SEND MESSAGE:0{}{}", TempClassroom->id, TempClassroom->progress));
                    }
                    Stu1Status = StudentStatus::WantToStudy;
                };
                if (FinishedClassroom >= 7)
                {
                    Stu1Status = StudentStatus::WantToGraduate;
                }
                break; }
            case StudentStatus::WantToGraduate://需要仿照wanttostudy写
            { TempGate = ChooseAGate(api, GridToCell(self->x), GridToCell(self->y));
            MapNode NewDestination1 = (*TempGate).xy;
            if (NewDestination1 != FinalDestination)
            {
                //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                pathnum = 0;
                pathlength = 0;
                memset(path, 0, 100);

                SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination1.x, NewDestination1.y);
            }
            if (MoveAlongPath(api))
            {
                Stu1Status = StudentStatus::Graduating;
            };
            break; }
            case StudentStatus::Graduating:
                //先更新大门的进度
            {(*TempGate).progress = api.GetGateProgress(ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.x, ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.y);
            if (TempGate->progress < 18000)
            {
                api.StartOpenGate();
            }
            else
            {
                api.Graduate();
            }
            break; }
            case StudentStatus::GoToWaitingPoint:
            {
                MoveAlongPath(api);
                break;
            }
            case StudentStatus::GoToStudy:
            {
                if (MoveAlongSettedPath(api))
                {
                    if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                    {
                        api.UseSkill(0);
                    }
                };
                break;
            }

            };
        }
        else
        {
            int ID = 0;
            auto self = api.GetSelfInfo();
            auto tricker = api.GetTrickers();
            auto GAME = api.GetGameInfo();
            api.Print(fmt::format("GameTime:{}", GAME->gameTime));

            NowArea = GetArea(GridToCell(self->x), GridToCell(self->y));//Wrong
            UpdateInfo(api);//Wrong
            api.Print(fmt::format("cell:{},{}", (Ppath[ID] + PathforLength[ID] - 1)->x, (Ppath[ID] + PathforLength[ID] - 1)->y));
            if (GAME->gameTime < 578000)
            {
                if (test)
                {
                    SetPath2(api, GridToCell(self->x), GridToCell(self->y), 6.2, 2.2);
                    test = false;
                }
                Stu1Status = StudentStatus::GoToWaitingPoint;
            }
            else
            {
                Stu1Status = StudentStatus::GoToStudy;
            }
            SetStatus(api);//空的
            DealingWithMessages(api);
            UsingSkills(api);//空的
            api.Print(fmt::format("Status:{}", (int)Stu1Status));

            switch (Stu1Status)
            {
            case StudentStatus::Init:
            {InitMap(api);
            Stu1Status = StudentStatus::WantToStudy;
            break; }

            case StudentStatus::WantToStudy:
            {

                TempClassroom = ChooseAClassroom(api, GridToCell(self->x), GridToCell(self->y), 0);
                api.Print(fmt::format("Tempclassroom{}", TempClassroom->id));
                MapNode NewDestination = TempClassroom->xy[this->playerID];
                if (NewDestination != FinalDestination)
                {
                    //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                    pathnum = 0;
                    pathlength = 0;
                    memset(path, 0, 100);

                    SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination.x, NewDestination.y);
                }
                if (MoveAlongPath(api))
                {
                    if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                    {
                        api.UseSkill(0);
                    }
                    Stu1Status = StudentStatus::Studying;
                };
                if (FinishedClassroom >= 7)
                {
                    Stu1Status = StudentStatus::WantToGraduate;
                }
                break; }

            case StudentStatus::Studying:
            {
                api.Print(fmt::format("TempClassroom:{},{},progress{}", TempClassroom->realxy.x, TempClassroom->realxy.y, TempClassroom->progress));
                TempClassroom->progress = api.GetClassroomProgress(TempClassroom->realxy.x, TempClassroom->realxy.y);
                if (TempClassroom->progress < 10000000)
                {
                    api.StartLearning();
                    //api.EndAllAction();
                }
                else
                {
                    for (int i = 0; i < 4; i++)
                    {
                        api.SendTextMessage(i, fmt::format("0{}{}", TempClassroom->id, TempClassroom->progress));
                        api.Print(fmt::format("SEND MESSAGE:0{}{}", TempClassroom->id, TempClassroom->progress));
                    }
                    Stu1Status = StudentStatus::WantToStudy;
                };
                if (FinishedClassroom >= 7)
                {
                    Stu1Status = StudentStatus::WantToGraduate;
                }
                break; }
            case StudentStatus::WantToGraduate://需要仿照wanttostudy写
            { TempGate = ChooseAGate(api, GridToCell(self->x), GridToCell(self->y));
            MapNode NewDestination1 = (*TempGate).xy;
            if (NewDestination1 != FinalDestination)
            {
                //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                pathnum = 0;
                pathlength = 0;
                memset(path, 0, 100);

                SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination1.x, NewDestination1.y);
            }
            if (MoveAlongPath(api))
            {
                Stu1Status = StudentStatus::Graduating;
            };
            break; }
            case StudentStatus::Graduating:
                //先更新大门的进度
            {(*TempGate).progress = api.GetGateProgress(ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.x, ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.y);
            if (TempGate->progress < 18000)
            {
                api.StartOpenGate();
            }
            else
            {
                api.Graduate();
            }
            break; }
            case StudentStatus::GoToWaitingPoint:
            {
                MoveAlongPath(api);
                break;
            }
            case StudentStatus::GoToStudy:
            {
                if (MoveAlongSettedPath2(api))
                {
                    if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                    {
                        api.UseSkill(0);
                    }
                };
                break;
            }

            };
        }
    }
    else if (this->playerID == 1)
    {
        InitMap(api);
        if (map[20][20] == THUAI6::PlaceType::Wall)
        {
            int ID = 1;
            auto self = api.GetSelfInfo();
            auto tricker = api.GetTrickers();
            auto GAME = api.GetGameInfo();
            api.Print(fmt::format("GameTime:{}", GAME->gameTime));

            NowArea = GetArea(GridToCell(self->x), GridToCell(self->y));
            UpdateInfo(api);
            api.Print(fmt::format("cell:{},{}", (Ppath[ID] + PathforLength[ID] - 1)->x, (Ppath[ID] + PathforLength[ID] - 1)->y));
            if (GAME->gameTime < 590000)
            {
                if (test)
                {
                    SetPath(api, GridToCell(self->x), GridToCell(self->y), 5, 34);
                    test = false;
                }
                Stu1Status = StudentStatus::GoToWaitingPoint;
            }
            else
            {
                Stu1Status = StudentStatus::GoToStudy;
            }
            SetStatus(api);//空的
            DealingWithMessages(api);
            UsingSkills(api);//空的
            api.Print(fmt::format("Status:{}", (int)Stu1Status));

            switch (Stu1Status)
            {
            case StudentStatus::Init:
            {InitMap(api);
            Stu1Status = StudentStatus::WantToStudy;
            break; }

            case StudentStatus::WantToStudy:
            {

                TempClassroom = ChooseAClassroom(api, GridToCell(self->x), GridToCell(self->y), 0);
                api.Print(fmt::format("Tempclassroom{}", TempClassroom->id));
                MapNode NewDestination = TempClassroom->xy[this->playerID];
                if (NewDestination != FinalDestination)
                {
                    //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                    pathnum = 0;
                    pathlength = 0;
                    memset(path, 0, 100);

                    SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination.x, NewDestination.y);
                }
                if (MoveAlongPath(api))
                {
                    if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                    {
                        api.UseSkill(0);
                    }
                    Stu1Status = StudentStatus::Studying;
                };
                if (FinishedClassroom >= 7)
                {
                    Stu1Status = StudentStatus::WantToGraduate;
                }
                break; }

            case StudentStatus::Studying:
            {
                api.Print(fmt::format("TempClassroom:{},{},progress{}", TempClassroom->realxy.x, TempClassroom->realxy.y, TempClassroom->progress));
                TempClassroom->progress = api.GetClassroomProgress(TempClassroom->realxy.x, TempClassroom->realxy.y);
                if (TempClassroom->progress < 10000000)
                {
                    api.StartLearning();
                    //api.EndAllAction();
                }
                else
                {
                    for (int i = 0; i < 4; i++)
                    {
                        api.SendTextMessage(i, fmt::format("0{}{}", TempClassroom->id, TempClassroom->progress));
                        api.Print(fmt::format("SEND MESSAGE:0{}{}", TempClassroom->id, TempClassroom->progress));
                    }
                    Stu1Status = StudentStatus::WantToStudy;
                };
                if (FinishedClassroom >= 7)
                {
                    Stu1Status = StudentStatus::WantToGraduate;
                }
                break; }
            case StudentStatus::WantToGraduate://需要仿照wanttostudy写
            { TempGate = ChooseAGate(api, GridToCell(self->x), GridToCell(self->y));
            MapNode NewDestination1 = (*TempGate).xy;
            if (NewDestination1 != FinalDestination)
            {
                //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                pathnum = 0;
                pathlength = 0;
                memset(path, 0, 100);

                SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination1.x, NewDestination1.y);
            }
            if (MoveAlongPath(api))
            {
                Stu1Status = StudentStatus::Graduating;
            };
            break; }
            case StudentStatus::Graduating:
                //先更新大门的进度
            {(*TempGate).progress = api.GetGateProgress(ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.x, ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.y);
            if (TempGate->progress < 18000)
            {
                api.StartOpenGate();
            }
            else
            {
                api.Graduate();
            }
            break; }
            case StudentStatus::GoToWaitingPoint:
            {
                MoveAlongPath(api);
                break;
            }
            case StudentStatus::GoToStudy:
            {
                if (MoveAlongSettedPath(api))
                {
                    if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                    {
                        api.UseSkill(0);
                    }
                };
                break;
            }

            };
        }
        else
        {
        int ID = 1;
        auto self = api.GetSelfInfo();
        auto tricker = api.GetTrickers();
        auto GAME = api.GetGameInfo();
        api.Print(fmt::format("GameTime:{}", GAME->gameTime));

        NowArea = GetArea(GridToCell(self->x), GridToCell(self->y));//Wrong
        UpdateInfo(api);//Wrong
        api.Print(fmt::format("cell:{},{}", (Ppath2[ID] + PathforLength[ID] - 1)->x, (Ppath2[ID] + PathforLength2[ID] - 1)->y));
        if (GAME->gameTime < 578000)
        {
            if (test)
            {
                SetPath2(api, GridToCell(self->x), GridToCell(self->y), 6, 47);
                test = false;
            }
            Stu1Status = StudentStatus::GoToWaitingPoint;
        }
        else
        {
            Stu1Status = StudentStatus::GoToStudy;
        }
        SetStatus(api);//空的
        DealingWithMessages(api);
        UsingSkills(api);//空的
        api.Print(fmt::format("Status:{}", (int)Stu1Status));

        switch (Stu1Status)
        {
        case StudentStatus::Init:
        {InitMap(api);
        Stu1Status = StudentStatus::WantToStudy;
        break; }

        case StudentStatus::WantToStudy:
        {

            TempClassroom = ChooseAClassroom(api, GridToCell(self->x), GridToCell(self->y), 0);
            api.Print(fmt::format("Tempclassroom{}", TempClassroom->id));
            MapNode NewDestination = TempClassroom->xy[this->playerID];
            if (NewDestination != FinalDestination)
            {
                //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                pathnum = 0;
                pathlength = 0;
                memset(path, 0, 100);

                SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination.x, NewDestination.y);
            }
            if (MoveAlongPath(api))
            {
                if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                {
                    api.UseSkill(0);
                }
                Stu1Status = StudentStatus::Studying;
            };
            if (FinishedClassroom >= 7)
            {
                Stu1Status = StudentStatus::WantToGraduate;
            }
            break; }

        case StudentStatus::Studying:
        {
            api.Print(fmt::format("TempClassroom:{},{},progress{}", TempClassroom->realxy.x, TempClassroom->realxy.y, TempClassroom->progress));
            TempClassroom->progress = api.GetClassroomProgress(TempClassroom->realxy.x, TempClassroom->realxy.y);
            if (TempClassroom->progress < 10000000)
            {
                api.StartLearning();
                //api.EndAllAction();
            }
            else
            {
                for (int i = 0; i < 4; i++)
                {
                    api.SendTextMessage(i, fmt::format("0{}{}", TempClassroom->id, TempClassroom->progress));
                    api.Print(fmt::format("SEND MESSAGE:0{}{}", TempClassroom->id, TempClassroom->progress));
                }
                Stu1Status = StudentStatus::WantToStudy;
            };
            if (FinishedClassroom >= 7)
            {
                Stu1Status = StudentStatus::WantToGraduate;
            }
            break; }
        case StudentStatus::WantToGraduate://需要仿照wanttostudy写
        { TempGate = ChooseAGate(api, GridToCell(self->x), GridToCell(self->y));
        MapNode NewDestination1 = (*TempGate).xy;
        if (NewDestination1 != FinalDestination)
        {
            //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
            pathnum = 0;
            pathlength = 0;
            memset(path, 0, 100);

            SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination1.x, NewDestination1.y);
        }
        if (MoveAlongPath(api))
        {
            Stu1Status = StudentStatus::Graduating;
        };
        break; }
        case StudentStatus::Graduating:
            //先更新大门的进度
        {(*TempGate).progress = api.GetGateProgress(ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.x, ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.y);
        if (TempGate->progress < 18000)
        {
            api.StartOpenGate();
        }
        else
        {
            api.Graduate();
        }
        break; }
        case StudentStatus::GoToWaitingPoint:
        {
            MoveAlongPath(api);
            break;
        }
        case StudentStatus::GoToStudy:
        {
            if (MoveAlongSettedPath2(api))
            {
                if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                {
                    api.UseSkill(0);
                }
            };
            break;
        }

        };
        }
    }
    else if (this->playerID == 2)
    {
        InitMap(api);
        if (map[20][20] == THUAI6::PlaceType::Wall)
        {
            int ID = 2;
            auto self = api.GetSelfInfo();
            auto tricker = api.GetTrickers();
            auto GAME = api.GetGameInfo();
            api.Print(fmt::format("GameTime:{}", GAME->gameTime));
            api.Wait();
            if (test2)
            {
                test2 = false;
                api.Move(3000, 3.92699);
            }
            NowArea = GetArea(GridToCell(self->x), GridToCell(self->y));
            UpdateInfo(api);
            api.Print(fmt::format("cell:{},{}", (Ppath[ID] + PathforLength[ID] - 1)->x, (Ppath[ID] + PathforLength[ID] - 1)->y));
            if (GAME->gameTime < 582000)
            {
                if (test)
                {
                    SetPath(api, GridToCell(self->x), GridToCell(self->y), 3, 8);
                    test = false;
                }
                Stu1Status = StudentStatus::GoToWaitingPoint;
            }
            else
            {
                Stu1Status = StudentStatus::GoToStudy;
            }
            SetStatus(api);//空的
            DealingWithMessages(api);
            UsingSkills(api);//空的
            api.Print(fmt::format("Status:{}", (int)Stu1Status));

            switch (Stu1Status)
            {
            case StudentStatus::Init:
            {InitMap(api);
            Stu1Status = StudentStatus::WantToStudy;
            break; }

            case StudentStatus::WantToStudy:
            {

                TempClassroom = ChooseAClassroom(api, GridToCell(self->x), GridToCell(self->y), 0);
                api.Print(fmt::format("Tempclassroom{}", TempClassroom->id));
                MapNode NewDestination = TempClassroom->xy[this->playerID];
                if (NewDestination != FinalDestination)
                {
                    //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                    pathnum = 0;
                    pathlength = 0;
                    memset(path, 0, 100);

                    SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination.x, NewDestination.y);
                }
                if (MoveAlongPath(api))
                {
                    if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                    {
                        api.UseSkill(0);
                    }
                    Stu1Status = StudentStatus::Studying;
                };
                if (FinishedClassroom >= 7)
                {
                    Stu1Status = StudentStatus::WantToGraduate;
                }
                break; }

            case StudentStatus::Studying:
            {
                api.Print(fmt::format("TempClassroom:{},{},progress{}", TempClassroom->realxy.x, TempClassroom->realxy.y, TempClassroom->progress));
                TempClassroom->progress = api.GetClassroomProgress(TempClassroom->realxy.x, TempClassroom->realxy.y);
                if (TempClassroom->progress < 10000000)
                {
                    api.StartLearning();
                    //api.EndAllAction();
                }
                else
                {
                    for (int i = 0; i < 4; i++)
                    {
                        api.SendTextMessage(i, fmt::format("0{}{}", TempClassroom->id, TempClassroom->progress));
                        api.Print(fmt::format("SEND MESSAGE:0{}{}", TempClassroom->id, TempClassroom->progress));
                    }
                    Stu1Status = StudentStatus::WantToStudy;
                };
                if (FinishedClassroom >= 7)
                {
                    Stu1Status = StudentStatus::WantToGraduate;
                }
                break; }
            case StudentStatus::WantToGraduate://需要仿照wanttostudy写
            { TempGate = ChooseAGate(api, GridToCell(self->x), GridToCell(self->y));
            MapNode NewDestination1 = (*TempGate).xy;
            if (NewDestination1 != FinalDestination)
            {
                //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                pathnum = 0;
                pathlength = 0;
                memset(path, 0, 100);

                SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination1.x, NewDestination1.y);
            }
            if (MoveAlongPath(api))
            {
                Stu1Status = StudentStatus::Graduating;
            };
            break; }
            case StudentStatus::Graduating:
                //先更新大门的进度
            {(*TempGate).progress = api.GetGateProgress(ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.x, ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.y);
            if (TempGate->progress < 18000)
            {
                api.StartOpenGate();
            }
            else
            {
                api.Graduate();
            }
            break; }
            case StudentStatus::GoToWaitingPoint:
            {
                MoveAlongPath(api);
                break;
            }
            case StudentStatus::GoToStudy:
            {
                if (MoveAlongSettedPath(api))
                {
                    if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                    {
                        api.UseSkill(0);
                    }
                };
                break;
            }

            };
        }
        else
        {
        int ID = 2;
        auto self = api.GetSelfInfo();
        auto tricker = api.GetTrickers();
        auto GAME = api.GetGameInfo();
        api.Print(fmt::format("GameTime:{}", GAME->gameTime));

        NowArea = GetArea(GridToCell(self->x), GridToCell(self->y));//Wrong
        UpdateInfo(api);//Wrong
        api.Print(fmt::format("cell:{},{}", (Ppath2[ID] + PathforLength[ID] - 1)->x, (Ppath2[ID] + PathforLength2[ID] - 1)->y));
        if (GAME->gameTime < 578000)
        {
            if (test)
            {
                SetPath2(api, GridToCell(self->x), GridToCell(self->y), 43,2);
                test = false;
            }
            Stu1Status = StudentStatus::GoToWaitingPoint;
        }
        else
        {
            Stu1Status = StudentStatus::GoToStudy;
        }
        SetStatus(api);//空的
        DealingWithMessages(api);
        UsingSkills(api);//空的
        api.Print(fmt::format("Status:{}", (int)Stu1Status));

        switch (Stu1Status)
        {
        case StudentStatus::Init:
        {InitMap(api);
        Stu1Status = StudentStatus::WantToStudy;
        break; }

        case StudentStatus::WantToStudy:
        {

            TempClassroom = ChooseAClassroom(api, GridToCell(self->x), GridToCell(self->y), 0);
            api.Print(fmt::format("Tempclassroom{}", TempClassroom->id));
            MapNode NewDestination = TempClassroom->xy[this->playerID];
            if (NewDestination != FinalDestination)
            {
                //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                pathnum = 0;
                pathlength = 0;
                memset(path, 0, 100);

                SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination.x, NewDestination.y);
            }
            if (MoveAlongPath(api))
            {
                if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                {
                    api.UseSkill(0);
                }
                Stu1Status = StudentStatus::Studying;
            };
            if (FinishedClassroom >= 7)
            {
                Stu1Status = StudentStatus::WantToGraduate;
            }
            break; }

        case StudentStatus::Studying:
        {
            api.Print(fmt::format("TempClassroom:{},{},progress{}", TempClassroom->realxy.x, TempClassroom->realxy.y, TempClassroom->progress));
            TempClassroom->progress = api.GetClassroomProgress(TempClassroom->realxy.x, TempClassroom->realxy.y);
            if (TempClassroom->progress < 10000000)
            {
                api.StartLearning();
                //api.EndAllAction();
            }
            else
            {
                for (int i = 0; i < 4; i++)
                {
                    api.SendTextMessage(i, fmt::format("0{}{}", TempClassroom->id, TempClassroom->progress));
                    api.Print(fmt::format("SEND MESSAGE:0{}{}", TempClassroom->id, TempClassroom->progress));
                }
                Stu1Status = StudentStatus::WantToStudy;
            };
            if (FinishedClassroom >= 7)
            {
                Stu1Status = StudentStatus::WantToGraduate;
            }
            break; }
        case StudentStatus::WantToGraduate://需要仿照wanttostudy写
        { TempGate = ChooseAGate(api, GridToCell(self->x), GridToCell(self->y));
        MapNode NewDestination1 = (*TempGate).xy;
        if (NewDestination1 != FinalDestination)
        {
            //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
            pathnum = 0;
            pathlength = 0;
            memset(path, 0, 100);

            SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination1.x, NewDestination1.y);
        }
        if (MoveAlongPath(api))
        {
            Stu1Status = StudentStatus::Graduating;
        };
        break; }
        case StudentStatus::Graduating:
            //先更新大门的进度
        {(*TempGate).progress = api.GetGateProgress(ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.x, ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.y);
        if (TempGate->progress < 18000)
        {
            api.StartOpenGate();
        }
        else
        {
            api.Graduate();
        }
        break; }
        case StudentStatus::GoToWaitingPoint:
        {
            MoveAlongPath(api);
            break;
        }
        case StudentStatus::GoToStudy:
        {
            if (MoveAlongSettedPath2(api))
            {
                if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                {
                    api.UseSkill(0);
                }
            };
            break;
        }

        };
        }
    }
    else if (this->playerID == 3)
    {
        InitMap(api);
        if (map[20][20] == THUAI6::PlaceType::Wall)
        {
            int ID = 3;
            auto self = api.GetSelfInfo();
            auto tricker = api.GetTrickers();
            auto GAME = api.GetGameInfo();
            api.Print(fmt::format("GameTime:{}", GAME->gameTime));

            NowArea = GetArea(GridToCell(self->x), GridToCell(self->y));
            UpdateInfo(api);
            api.Print(fmt::format("cell:{},{}", (Ppath[ID] + PathforLength[ID] - 1)->x, (Ppath[ID] + PathforLength[ID] - 1)->y));
            if (GAME->gameTime < 586000)
            {
                if (test)
                {
                    SetPath(api, GridToCell(self->x), GridToCell(self->y), 43, 43);
                    test = false;
                }
                Stu1Status = StudentStatus::GoToWaitingPoint;
            }
            else
            {
                Stu1Status = StudentStatus::GoToStudy;
            }
            SetStatus(api);//空的
            DealingWithMessages(api);
            UsingSkills(api);//空的
            api.Print(fmt::format("Status:{}", (int)Stu1Status));

            switch (Stu1Status)
            {
            case StudentStatus::Init:
            {InitMap(api);
            Stu1Status = StudentStatus::WantToStudy;
            break; }

            case StudentStatus::WantToStudy:
            {

                TempClassroom = ChooseAClassroom(api, GridToCell(self->x), GridToCell(self->y), 0);
                api.Print(fmt::format("Tempclassroom{}", TempClassroom->id));
                MapNode NewDestination = TempClassroom->xy[this->playerID];
                if (NewDestination != FinalDestination)
                {
                    //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                    pathnum = 0;
                    pathlength = 0;
                    memset(path, 0, 100);

                    SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination.x, NewDestination.y);
                }
                if (MoveAlongPath(api))
                {
                    if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                    {
                        api.UseSkill(0);
                    }
                    Stu1Status = StudentStatus::Studying;
                };
                if (FinishedClassroom >= 7)
                {
                    Stu1Status = StudentStatus::WantToGraduate;
                }
                break; }

            case StudentStatus::Studying:
            {
                api.Print(fmt::format("TempClassroom:{},{},progress{}", TempClassroom->realxy.x, TempClassroom->realxy.y, TempClassroom->progress));
                TempClassroom->progress = api.GetClassroomProgress(TempClassroom->realxy.x, TempClassroom->realxy.y);
                if (TempClassroom->progress < 10000000)
                {
                    api.StartLearning();
                    //api.EndAllAction();
                }
                else
                {
                    for (int i = 0; i < 4; i++)
                    {
                        api.SendTextMessage(i, fmt::format("0{}{}", TempClassroom->id, TempClassroom->progress));
                        api.Print(fmt::format("SEND MESSAGE:0{}{}", TempClassroom->id, TempClassroom->progress));
                    }
                    Stu1Status = StudentStatus::WantToStudy;
                };
                if (FinishedClassroom >= 7)
                {
                    Stu1Status = StudentStatus::WantToGraduate;
                }
                break; }
            case StudentStatus::WantToGraduate://需要仿照wanttostudy写
            { TempGate = ChooseAGate(api, GridToCell(self->x), GridToCell(self->y));
            MapNode NewDestination1 = (*TempGate).xy;
            if (NewDestination1 != FinalDestination)
            {
                //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                pathnum = 0;
                pathlength = 0;
                memset(path, 0, 100);

                SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination1.x, NewDestination1.y);
            }
            if (MoveAlongPath(api))
            {
                Stu1Status = StudentStatus::Graduating;
            };
            break; }
            case StudentStatus::Graduating:
                //先更新大门的进度
            {(*TempGate).progress = api.GetGateProgress(ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.x, ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.y);
            if (TempGate->progress < 18000)
            {
                api.StartOpenGate();
            }
            else
            {
                api.Graduate();
            }
            break; }
            case StudentStatus::GoToWaitingPoint:
            {
                MoveAlongPath(api);
                break;
            }
            case StudentStatus::GoToStudy:
            {
                if (MoveAlongSettedPath(api))
                {
                    if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                    {
                        api.UseSkill(0);
                    }
                };
                break;
            }

            };
        }
        else
        {
        int ID = 3;
        auto self = api.GetSelfInfo();
        auto tricker = api.GetTrickers();
        auto GAME = api.GetGameInfo();
        api.Print(fmt::format("GameTime:{}", GAME->gameTime));

        NowArea = GetArea(GridToCell(self->x), GridToCell(self->y));//Wrong
        UpdateInfo(api);//Wrong
        api.Print(fmt::format("cell:{},{}", (Ppath2[ID] + PathforLength[ID] - 1)->x, (Ppath2[ID] + PathforLength2[ID] - 1)->y));
        if (GAME->gameTime < 578000)
        {
            if (test)
            {
                SetPath2(api, GridToCell(self->x), GridToCell(self->y), 43,47);
                test = false;
            }
            Stu1Status = StudentStatus::GoToWaitingPoint;
        }
        else
        {
            Stu1Status = StudentStatus::GoToStudy;
        }
        SetStatus(api);//空的
        DealingWithMessages(api);
        UsingSkills(api);//空的
        api.Print(fmt::format("Status:{}", (int)Stu1Status));

        switch (Stu1Status)
        {
        case StudentStatus::Init:
        {InitMap(api);
        Stu1Status = StudentStatus::WantToStudy;
        break; }

        case StudentStatus::WantToStudy:
        {

            TempClassroom = ChooseAClassroom(api, GridToCell(self->x), GridToCell(self->y), 0);
            api.Print(fmt::format("Tempclassroom{}", TempClassroom->id));
            MapNode NewDestination = TempClassroom->xy[this->playerID];
            if (NewDestination != FinalDestination)
            {
                //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
                pathnum = 0;
                pathlength = 0;
                memset(path, 0, 100);

                SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination.x, NewDestination.y);
            }
            if (MoveAlongPath(api))
            {
                if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                {
                    api.UseSkill(0);
                }
                Stu1Status = StudentStatus::Studying;
            };
            if (FinishedClassroom >= 7)
            {
                Stu1Status = StudentStatus::WantToGraduate;
            }
            break; }

        case StudentStatus::Studying:
        {
            api.Print(fmt::format("TempClassroom:{},{},progress{}", TempClassroom->realxy.x, TempClassroom->realxy.y, TempClassroom->progress));
            TempClassroom->progress = api.GetClassroomProgress(TempClassroom->realxy.x, TempClassroom->realxy.y);
            if (TempClassroom->progress < 10000000)
            {
                api.StartLearning();
                //api.EndAllAction();
            }
            else
            {
                for (int i = 0; i < 4; i++)
                {
                    api.SendTextMessage(i, fmt::format("0{}{}", TempClassroom->id, TempClassroom->progress));
                    api.Print(fmt::format("SEND MESSAGE:0{}{}", TempClassroom->id, TempClassroom->progress));
                }
                Stu1Status = StudentStatus::WantToStudy;
            };
            if (FinishedClassroom >= 7)
            {
                Stu1Status = StudentStatus::WantToGraduate;
            }
            break; }
        case StudentStatus::WantToGraduate://需要仿照wanttostudy写
        { TempGate = ChooseAGate(api, GridToCell(self->x), GridToCell(self->y));
        MapNode NewDestination1 = (*TempGate).xy;
        if (NewDestination1 != FinalDestination)
        {
            //Goto(api, path[pathnum - 2].x, path[pathnum - 2].y);
            pathnum = 0;
            pathlength = 0;
            memset(path, 0, 100);

            SetPath(api, GridToCell(self->x), GridToCell(self->y), NewDestination1.x, NewDestination1.y);
        }
        if (MoveAlongPath(api))
        {
            Stu1Status = StudentStatus::Graduating;
        };
        break; }
        case StudentStatus::Graduating:
            //先更新大门的进度
        {(*TempGate).progress = api.GetGateProgress(ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.x, ChooseAGate(api, GridToCell(self->x), GridToCell(self->y))->xy.y);
        if (TempGate->progress < 18000)
        {
            api.StartOpenGate();
        }
        else
        {
            api.Graduate();
        }
        break; }
        case StudentStatus::GoToWaitingPoint:
        {
            MoveAlongPath(api);
            break;
        }
        case StudentStatus::GoToStudy:
        {
            if (MoveAlongSettedPath2(api))
            {
                if (self->studentType == THUAI6::StudentType::StraightAStudent)//学霸使用技能
                {
                    api.UseSkill(0);
                }
            };
            break;
        }

        };
        }
    }
}

int WantArea = 0;//用于Tricker的Searching状态调试
bool IsinCircle = true;//用于判断tricker能不能沿着规定的Circle走。


void AI::play(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    auto students = api.GetStudents();
    api.Print(fmt::format("TrickerDesire{}",self->trickDesire) );
    SetStatus(api);
    api.Print(fmt::format("CloseStudent:{},{}", GridToCell(ClosedStudent.x), GridToCell(ClosedStudent.y)));
    NowArea = GetArea(GridToCell(self->x), GridToCell(self->y));
    InitMap(api);
    if (self->trickDesire > 1.176)
    {
        api.UseSkill(0);
    }
    if(map[20][20]==THUAI6::PlaceType::Wall)

    {
        switch (Tricker1Status)
        {
        case TrickerStatus::Init:
        {InitMap(api);
        Tricker1Status = TrickerStatus::Searching;
        break; }
        case TrickerStatus::Tracing:
        {
            int nowCount = api.GetFrameCount();//这一部分用来实现翻墙
            api.Print(fmt::format("{},{}", nowCount, lastSkipCount));
            if (nowCount - lastSkipCount > 100)
            {
                if (api.SkipWindow().get())
                {
                    pathnum++;
                    Goto(api, path[pathnum].x, path[pathnum].y);
                    lastSkipCount = nowCount;
                }
            }
            if (!BeStuck(api))
                GotoGrid(api, ClosedStudent.x, ClosedStudent.y);
            else//处理被卡住的情况
            {
                int selfx = self->x;
                int selfy = self->y;
                auto delta_x = ClosedStudent.x - selfx;
                auto delta_y = ClosedStudent.y - selfy;
                double ang = 0;
                ang = atan2(delta_y, delta_x);
                if (Stuck)
                {
                    ang += PI / 2.0;
                    Stuck = false;
                }
                else
                {
                    ang -= PI / 2.0;
                    Stuck = true;
                }
                api.Move(80, ang);
            }

            break; }
        case TrickerStatus::Attaking:
        {
            if (self->timeUntilSkillAvailable[0] == 0)
            {
                api.UseSkill(0);
            }
            if (students.size() > 0)
                api.Attack(atan2(ClosedStudent.y - self->y, ClosedStudent.x - self->x));
            else
                api.Attack(atan2(ClosedTrickerPath[pathnum].y - GridToCell(self->y), ClosedTrickerPath[pathnum].x - GridToCell(self->x)));
            GotoGrid(api, ClosedStudent.x, ClosedStudent.y);
            api.Print("Attacking!and Moving!");
            break;
        }
        case TrickerStatus::Searching:
            //如果在TB1而且是不知道去哪，就选个TB前往，如果到达TB3,5,6中的指定点，就更改一个其他TB的目的地，每帧都会移动
        {
            if (!StatusIsSearching)
            {
                int closednumber = -1;
                int mindistance = 100, tempdistance;
                for (int i = 0; i < 56; i++)
                {
                    tempdistance = GetDistance(GridToCell(self->x), GridToCell(self->y), ClosedTrickerPath[i].x, ClosedTrickerPath[i].y);
                    if ((tempdistance < mindistance) && !isWalled(GridToCell(self->x), GridToCell(self->y), ClosedTrickerPath[i].x, ClosedTrickerPath[i].y))
                    {

                        mindistance = tempdistance;
                        closednumber = i;
                    }
                    IsinCircle = true;

                }
                pathnum = closednumber;
                if (closednumber == -1)
                {
                    IsinCircle = false;
                    SetPath(api, GridToCell(self->x), GridToCell(self->y), 22, 25);
                }
                api.Print(fmt::format("ClosedNumberInClosePath:{}", closednumber));

            }
            if (IsinCircle)
            {
                api.Print("isincircle");
                MoveAlongClosedPath(api);
                api.Print(fmt::format("ClosePathnum:{}", pathnum));
            }
            else
            {
                api.Print("notincircle");
                if (MoveAlongPath(api)||arriveAt(api,22,25))
                {

                    pathnum = 0;
                    IsinCircle = true;
                    Arrived = false;
                }

            }

            StatusIsSearching = true;

            break;
        }
        }

        
    }
    else
    {
        api.Print("MAP 2 !!!");
        switch (Tricker1Status)
        {
        case TrickerStatus::Init:
        {InitMap(api);
        Tricker1Status = TrickerStatus::Searching;
        break; }
        case TrickerStatus::Tracing:
        {
            int nowCount = api.GetFrameCount();//这一部分用来实现翻墙
            api.Print(fmt::format("{},{}", nowCount, lastSkipCount));
            if (nowCount - lastSkipCount > 100)
            {
                if (api.SkipWindow().get())
                {
                    pathnum++;
                    Goto(api, path[pathnum].x, path[pathnum].y);
                    lastSkipCount = nowCount;
                }
            }
            if (!BeStuck(api))
                GotoGrid(api, ClosedStudent.x, ClosedStudent.y);
            else//处理被卡住的情况
            {
                int selfx = self->x;
                int selfy = self->y;
                auto delta_x = ClosedStudent.x - selfx;
                auto delta_y = ClosedStudent.y - selfy;
                double ang = 0;
                ang = atan2(delta_y, delta_x);
                if (Stuck)
                {
                    ang += PI / 2.0;
                    Stuck = false;
                }
                else
                {
                    ang -= PI / 2.0;
                    Stuck = true;
                }
                api.Move(80, ang);
            }

            break; }
        case TrickerStatus::Attaking:
        {
            if (self->timeUntilSkillAvailable[0] == 0)
            {
                api.UseSkill(0);
            }
            api.Attack(atan2(ClosedStudent.y - self->y, ClosedStudent.x - self->x));
            GotoGrid(api, ClosedStudent.x, ClosedStudent.y);
            api.Print("Attacking!and Moving!");
            break;
        }
        case TrickerStatus::Searching:
            
        {
            if (!StatusIsSearching)
            {
                int closednumber = -1;
                int mindistance = 100, tempdistance;
                for (int i = 0; i < 68; i++)
                {
                    tempdistance = GetDistance(GridToCell(self->x), GridToCell(self->y), ClosedTrickerPath2[i].x, ClosedTrickerPath2[i].y);
                    if ((tempdistance < mindistance) && !isWalled(GridToCell(self->x),GridToCell( self->y), ClosedTrickerPath2[i].x, ClosedTrickerPath2[i].y))
                    {

                        mindistance = tempdistance;
                        closednumber = i;
                    }
                    IsinCircle = true;

                }
                pathnum = closednumber;
                if (closednumber == -1)
                {
                    IsinCircle = false;
                    SetPath(api, GridToCell(self->x), GridToCell(self->y), 25, 25);//这句慎重
                }
                api.Print(fmt::format("ClosedNumberInClosePath:{}", closednumber));

            }
            if (IsinCircle)
            {
                api.Print("isincircle");
                MoveAlongClosedPath2(api);
            }
            else
            {
                api.Print("notincircle");
                if (MoveAlongPath(api))
                {
                    pathnum = 0;
                    IsinCircle = true;
                    Arrived = false;
                }

            }

            StatusIsSearching = true;

            break;
        }
        }


    }
}
