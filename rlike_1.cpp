#include <regex>
#include <random>
#include <algorithm>
#include <iostream>
#include <iomanip>      // Manipulators for iostream.
#include <memory>
#include <cctype>
#include <deque>
#include <map>
#include <set>

static std::mt19937 rnd;    // Random number engine (Mersenne twister algorithm).

// FRAND() generates a random number between 0 and 1.
#define FRAND()     std::uniform_real_distribution<>(0.f, 1.f)(rnd)
// Generate a random number of specified range.
#define RAND(size)  std::uniform_int_distribution<>(0, (size)-1)(rnd)
// Return the size of an array at compile time.
template<typename T, std::size_t size>
constexpr std::size_t Count(const T (&arr)[size]) {
    return size;
}

// Syntactic shorthand for creating regular expressions.
static std::regex operator ""_r(const char* pattern, std::size_t length) {
    return std::regex(pattern, length);
}



/* First, define some stuff for our adventure. */
struct GenericData {
    // These three generic fields will provide most of our data needs.
    const char* name;
    float worth;
    float weight;
} static const
// Define types of coins. Reference value: 1.0 = gold. Each coin weighs 0.01 units.
moneyTypes[] = {
    {"platinum",  10, 0.01}, {"gold"  ,   1, 0.01}, {"silver",  0.6, 0.01},
    {"bronze"  , 0.4, 0.01}, {"copper", 0.2, 0.01}, {"wood"  , 0.01, 0.01}
},
// List of different kinds of tunnels. It is just for variance.
envTypes[] = {
    {"dark"      , 0, 0}, {"tall"  , 0, 0}, {"humid", 0, 0},
    {"beautifull", 0, 0}, {"narrow", 0, 0}
};



// Any particular room in the puzzle may contain the following (Every cell is a room):
struct Room {
    std::size_t wall = 0, env = 0;      // Indexs.
    unsigned seed = 0;                  // For maze generation.
// Create a model "default" room based on empty definitions.
} static const defaultRoom;

struct Maze {
    // A maze contains rooms.
    std::map<long/*x*/, std::map<long/*y*/, Room>> rooms;

    // Generate a room at given coordinates.
    // The "model" room will help the maze generator generate similar rooms in nearby locations.
    Room& GenerateRoom(long x, long y, const Room& model, unsigned seed) {
        // Reinitalizes the internal state of the random-number engine using new seed value.
        rnd.seed( y*0xC70F6907UL + x*2166136261UL );
        // Function insert returns a pair:
        // first object is a pointer point to the insertion(also a pair).
        // second object is a bool value, true on sucesss, false on failure.
        auto insRes = rooms[x].insert( {y, model} );
        Room& room = insRes.first->second;
        if (insRes.second) {
            // If a new room was indeed inserted, make changes in it.
            room.seed = (seed + (FRAND() > 0.95 ? RAND(4) : 0)) & 3;
            // 10% chance for the environment type to change.
            if (FRAND() > 0.9) room.env = RAND(Count(envTypes));
            if (FRAND() > (seed == model.seed ? 0.95 : 0.1)) room.wall = FRAND() < 0.4 ? 2 : 0;
        }
        return room;
    }
    // Describe the room with a single character.
    char Char(long x, long y) const {
        // Function find returns a pointer point to the target pair:
        // The pair's first object equals the parameter passed to the find funcion.
        // The second object is the value we would like to get.
        auto i = rooms.find(x);     if (i == rooms.end())     return ' ';
        auto j = i->second.find(y); if (j == i->second.end()) return ' ';
        if (j->second.wall)         return '#';
        return '.';
    }
} static maze;



// Player's location and life.
static long x = 0, y = 0, life = 1000;



static bool CanMoveTo(long whereX, long whereY, const Room& model = defaultRoom) {
    if (!maze.GenerateRoom(whereX, whereY, model, 0).wall) return true;
    return false;
}

static Room& SpawnRooms(long whereX, long whereY, const Room& model = defaultRoom) {
    Room& room = maze.GenerateRoom(whereX, whereY, model, 0);
    // Use this algorithm to generate 4 rooms around (x,y),
    // the seeds of these rooms are 1, 2, 3, 4 respectively.
    #define SPAWN_4_ROOMS(x, y) \
        for (char p : {1, 3, 5, 7}) \
            maze.GenerateRoom(x + p%3-1, y + p/3-1, room, (p+1)/2)
    SPAWN_4_ROOMS(whereX, whereY);
    for (int o=1; o<5 && CanMoveTo(whereX, whereY+o, room); ++o) SPAWN_4_ROOMS(whereX, whereY+o);
    for (int o=1; o<5 && CanMoveTo(whereX, whereY-o, room); ++o) SPAWN_4_ROOMS(whereX, whereY-o);
    for (int o=1; o<6 && CanMoveTo(whereX+o, whereY, room); ++o) SPAWN_4_ROOMS(whereX+o, whereY);
    for (int o=1; o<6 && CanMoveTo(whereX-o, whereY, room); ++o) SPAWN_4_ROOMS(whereX-o, whereY);
    return room;
}

// This routine is responsible for providing the view for the player.
// It also generates new maze data.
static void Look() {
    // Generate rooms in the field of vision of the player.
    const Room& room = SpawnRooms(x, y);

    // Generate the current map view.
    std::vector<std::string> mapGraph;
    for (long yo=-4; yo<=4; ++yo) {
        std::string line;
        for (long xo=-5; xo<=5; ++xo) {
            char c = ((xo==0 && yo==0) ? '@' : maze.Char(x+xo, y+yo));
            line += c;
        }
        mapGraph.push_back(line);
    }

    // This is the text that will be printed on the right side of the map.
    std::ostringstream oss;
    oss << "In a "  << envTypes[room.env].name << " tunnel at " << std::setw(3) << x << "," << std::setw(3) << y << "\n"
        << "Exits:" << (CanMoveTo(x,y-1) ? " north" : "") << (CanMoveTo(x,y+1) ? " south" : "")
                    << (CanMoveTo(x-1,y) ? " west"  : "") << (CanMoveTo(x+1,y) ? " east"  : "") << "\n\n";
    const std::string infoStr = oss.str();

    // Print the map and the information side by side.
    auto m = mapGraph.begin();
    auto b = infoStr.begin(), e = infoStr.end();
    auto pat = "([^\n]*)\n"_r;  // There're two regexs: "[^\n]*\n" and "[^\n]*".
    for (std::smatch res; m!=mapGraph.end() || b!=e; res=std::smatch{}) {
        // res[0] matchs "[^\n]*\n", res[1] matchs "[^\n]*".
        // In res[0]: first object means the start of matching sequence.
        //            second object means the end of matching sequence (The start of res[1]).
        if (b != e) { std::regex_search(b, e, res, pat); b = res[0].second; }
        std::string sa = m!=mapGraph.end() ? *m++ : std::string(11, ' ');
        std::string sb = res.ready() ? (std::string)res[1] : "";
        std::cout << sa << " | " << sb << "\n";
    }
}

static void EatLife(long l) {
    const char* msg = nullptr;
    if (life>=800 && life-l<800) msg = "You are so hungry!\n";
    if (life>=150 && life-l<150) msg = "You are famished!\n";
    if (life>=70  && life-l<70)  msg = "You are about to collapse any second!\n";
    life -= 1;
    if (msg) { std::cout << msg; }
}

static bool TryMoveBy(int xd, int yd) {
    // If we are moving diagonally, ensure that there is an actual path.
    if (!CanMoveTo(x+xd, y+yd) || (!CanMoveTo(x, y+yd) && !CanMoveTo(x+xd, y)))
        { std::cout << "You cannot go that way.\n"; return false; }

    long burden = 1;
    x += xd; y += yd;
    EatLife(burden);

    return true;
}



int main() {
    std::cout << "Welcome to the treasure dungeon.\n";

help:
    std::cout << "\nAvailable commands:\n"
                 "\tl/look\n"
                 "\tn/s/w/e for moving\n"
                 "\tquit\n"
                 "\thelp\n\n"
                 "You are starving. You are trying to find enough stuff to sell\n"
                 "for food before you die. Beware, food is very expensive here.\n\n";


    // The main loop.
    Look();
    while (life > 0) {
        // Produce the prompt and wait for player's command.
        std::cout << "[life:" << life << "]" << std::flush;
        std::string s;
        std::getline(std::cin, s);
        if (!std::cin.good() || s == "quit") break;
        if (s.empty()) continue;

        // Parse the command using C++11 regex.
        std::smatch res;

        #define RM std::regex_match

        // First, some metacommands.
        if (RM(s, R"((?:help|what|\?))"_r)) goto help;

        // Some fundamental movement commands.
        else if (RM(s, "((go|walk|move) +)?(n|north)"_r))      { if (TryMoveBy( 0,-1)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(s|south)"_r))      { if (TryMoveBy( 0, 1)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(w|west)"_r))       { if (TryMoveBy(-1, 0)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(e|east)"_r))       { if (TryMoveBy( 1, 0)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(nw|northwest)"_r)) { if (TryMoveBy(-1,-1)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(ne|northeast)"_r)) { if (TryMoveBy( 1,-1)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(sw|southwest)"_r)) { if (TryMoveBy(-1, 1)) Look(); }
        else if (RM(s, "((go|walk|move) +)?(se|southeast)"_r)) { if (TryMoveBy( 1, 1)) Look(); }

        // Then commands for looking at things.
        // Use the power of regex to recognize complex syntax.
        else if (RM(s, "(?:l|look)( +around)?"_r)) Look();

        else if (RM(s, R"((?:wear|wield|eq)\b.*)"_r))
            std::cout << "You are scavenging for survival and not playing an RPG character.\n";
        else if (RM(s, R"(eat\b.*)"_r))
            std::cout << "You have nothing edible! You are hoping to collect something you can sell for food.\n";

        // Any unrecognized command.
        else std::cout << "what\n";
    }

    std::cout << (life < 0 ? "You are pulled out from the maze by a supernatural force!" : "byebye")
              << "[life:" << life << "] Game over\n";
}