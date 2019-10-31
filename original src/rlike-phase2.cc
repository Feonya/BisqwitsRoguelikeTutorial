#include <regex>
#include <random>
#include <algorithm>
#include <iostream>
#include <memory>
#include <cctype>
#include <deque>
#include <map>
#include <set>

#include "printf.hh"

static std::mt19937 rnd;

// frand() generates a random number between 0 and 1.
#define frand()    std::uniform_real_distribution<>(0.f, 1.f)(rnd)
// Generate a random number of specified range.
#define rand(size) std::uniform_int_distribution<>(0, (size)-1)(rnd)
// Return the size of an array at compile time.
template<typename T, std::size_t size>
constexpr std::size_t count(const T(&) [size])
{
    return size;
}

// Syntactic shorthand for creating regular expressions.
static std::regex operator ""_r(const char* pattern, std::size_t length)
{
    return std::regex(pattern,length);
}



/* First, define some stuff for our adventure. */
struct GenericData
{
    // These three generic fields will provide most of our data needs.
    const char* name;
    float worth;
    float weight;
} static const
// Define types of coins. Reference value: 1.0 = gold. Each coin weighs 0.01 units.
MoneyTypes[] =
    { {"platinum",10, 0.01},    {"gold",     1, 0.01},    {"silver", 0.6,  0.01},
      {"bronze", 0.4, 0.01},    {"copper", 0.2, 0.01},    {"wood",   0.01, 0.01} },
// List of different kinds of tunnels. It is just for variance.
EnvTypes[] =
    { {"dark",     0,0},        {"tall",   0,0},          {"humid", 0,0},
      {"beautiful",0,0},        {"narrow", 0,0} };



// Any particular room in the puzzle may contain the following:
struct Room
{
    std::size_t Wall=0, Env=0;  // Indexes
    unsigned seed = 0;          // For maze generation

// Create a model "default" room based on empty definitions.
} static const defaultroom;


struct Maze
{
    // A maze contains rooms.
    std::map<long/*x*/,std::map<long/*y*/,Room> > rooms;

    // Generate a room at given coordinates.
    // The "model" room will help the maze generator generate
    // similar rooms in nearby locations.
    Room& GenerateRoom(long x,long y, const Room& model, unsigned seed)
    {
        rnd.seed( y*0xc70f6907UL + x*2166136261UL );
        auto insres = rooms[x].insert( {y, model} );
        Room& room = insres.first->second;
        if(insres.second)
        {
            // If a new room was indeed inserted, make changes in it.
            room.seed  = (seed + (frand() > 0.95 ? rand(4) : 0)) & 3;
            // 10% chance for the environment type to change.
            if(frand() > 0.9) room.Env = rand(count(EnvTypes));
            if(frand() > (seed==model.seed ? 0.95 : 0.1))
                room.Wall = frand() < 0.4 ? 2 : 0;
        }
        return room;
    }
    // Describe the room with a single character.
    char Char(long x,long y) const
    {
        auto i = rooms.find(x);     if(i == rooms.end())     return ' ';
        auto j = i->second.find(y); if(j == i->second.end()) return ' ';
        if(j->second.Wall)          return '#';
        return '.';
    }
} static maze;


// Player's location and life.
static long x=0, y=0, life=1000;


static bool CanMoveTo(long wherex,long wherey, const Room& model = defaultroom)
{
    if(!maze.GenerateRoom(wherex, wherey, model, 0).Wall) return true;
    return false;
}

static Room& SpawnRooms(long wherex,long wherey, const Room& model = defaultroom)
{
    Room& room = maze.GenerateRoom(wherex,wherey, model, 0);
    #define Spawn4rooms(x,y) \
        for(char p: { 1,3,5,7 }) \
            maze.GenerateRoom(x + p%3-1, y + p/3-1, room, (p+1)/2)
    Spawn4rooms(wherex,wherey);
    for(int o=1; o<5 && CanMoveTo(wherex,wherey+o, room); ++o) Spawn4rooms(wherex,wherey+o);
    for(int o=1; o<5 && CanMoveTo(wherex,wherey-o, room); ++o) Spawn4rooms(wherex,wherey-o);
    for(int o=1; o<6 && CanMoveTo(wherex-o,wherey, room); ++o) Spawn4rooms(wherex-o,wherey);
    for(int o=1; o<6 && CanMoveTo(wherex+o,wherey, room); ++o) Spawn4rooms(wherex+o,wherey);
    return room;
}

// This routine is responsible for providing the view for the player.
// It also generates new maze data.
static void Look()
{
    // Generate rooms in the field of vision of the player.
    const Room& room = SpawnRooms(x,y);

    // Generate the current map view
    std::vector<std::string> mapgraph;
    for(long yo=-4; yo<=4; ++yo)
    {
        std::string line;
        for(long xo=-5; xo<=5; ++xo)
        {
            char c = ((xo==0&&yo==0) ? '@' : maze.Char(x+xo, y+yo));
            line += c;
        }
        mapgraph.push_back(line);
    }

    // This is the text that will be printed on the right side of the map
    const std::string info_str =
        "In a %s tunnel at %+3ld,%+3ld\n"_f % EnvTypes[room.Env].name % x % -y
      + "Exits:%s%s%s%s\n\n"_f
        % (CanMoveTo(x+0, y-1) ? " north" : "")
        % (CanMoveTo(x+0, y+1) ? " south" : "")
        % (CanMoveTo(x-1, y+0) ? " west" : "")
        % (CanMoveTo(x+1, y+0) ? " east" : "");

    // Print the map and the information side by side.
    auto m = mapgraph.begin();
    auto b = info_str.begin(), e = info_str.end();
    auto pat = "([^\n]*)\n"_r;
    for(std::smatch res; m != mapgraph.end() || b != e; res = std::smatch{})
    {
        if(b != e) { std::regex_search(b, e, res, pat); b = res[0].second; }
        std::string sa = m!=mapgraph.end() ? *m++ : std::string(11,' ');
        std::string sb = res[1];
        std::cout << "%s | %s\n"_f % sa % sb;
    }
}

static void EatLife(long l)
{
    const char* msg = nullptr;
    if(life>=800 && life-l<800) msg = "You are so hungry!\n";
    if(life>=150 && life-l<150) msg = "You are famished!\n";
    if(life>=70 && life-l<70) msg = "You are about to collapse any second!\n";
    life -= l;
    if(msg) { std::cout << msg; }
}

static bool TryMoveBy(int xd,int yd)
{
    // If we are moving diagonally, ensure that there is an actual path.
    if(!CanMoveTo(x+xd, y+yd) || (!CanMoveTo(x,y+yd) && !CanMoveTo(x+xd,y)))
        { std::cout << "You cannot go that way.\n"; return false; }

    long burden = 1;
    x += xd;
    y += yd;
    EatLife(burden);

    return true;
}


struct Alias
{
    std::regex  pattern;
    std::string replacement;
} static const aliases[] =
{
    { R"(^l\b)"_r,                     "look"     },
    { R"(^lat? )"_r,                   "look at " },
    { R"(^lin? )"_r,                   "look in " },
    { R"(^look in )"_r,                "look at all in " },
    { R"(^ga\b)"_r,                    "get all"  },
    { R"(^da\b)"_r,                    "drop all" },
    { R"(^d )"_r,                      "drop "    },
    { R"(^g )"_r,                      "get "     },
    { R"(^take )"_r,                   "get "     },
    { R"(^pry )"_r,                    "open "    },
    { R"(^i\b)"_r,                     "inv"      },
    { R"(^inventory\b)"_r,             "inv"      },
    { R"(^da\b)"_r,                    "drop all" },
    { R"(^put(.*)\b(in|into|to)\b)"_r, "drop$1in" },
    { R"(\busing\b)"_r,                "with"     },
    { R"(\bwith my\b)"_r,              "with"     },
    { R"(^\s+)"_r,                     ""         },
    { R"(\s+$)"_r,                     ""         }
};

// A command line history and input engine.
struct CommandReader
{
    enum : unsigned { HistLen = 10, HistMin = 5 };

    std::deque<std::string> history;
    std::string prompt;
    std::pair<std::string, unsigned> repeat;

    void SetPrompt(const std::string& s) { prompt = s; }

    std::string ReadCommand()
    {
        for(;;)
        {
            std::cout << prompt;
            std::cout << std::flush;

            std::string cmd;

            if(repeat.second)
            {
                --repeat.second;
                cmd = repeat.first;
            }
            else
            {
                std::getline(std::cin, cmd);
                if(!std::cin.good()) return "quit";
            }
            if(cmd.empty()) continue;

            // Check if the command begins with a number, indicating
            // a desire to repeat a command a number of times.
            std::smatch res;
            if(std::regex_match(cmd, res, "^([1-9][0-9]*) +([^ 1-9].*)"_r))
            {
                repeat = { res[2], std::stoi(res[1]) };
                if(repeat.second > 50)
                {
                    std::cout << "Ignoring too large repeat count %u\n"_f % repeat.second;
                    repeat.second = 0;
                }
                continue;
            }

            // Add every command to the history
            if(cmd[0] != '!' && !repeat.second && cmd.size() >= HistMin)
            {
                history.push_back(cmd);
                if(history.size() > HistLen) history.pop_front();
            }

            // Deal with history searches
            if(cmd[0] == '!' && cmd != "!?")
            {
                for(std::size_t a=history.size(); a-- > 0; )
                    if(history[a].compare(0, cmd.size()-1, cmd, 1, cmd.size()-1)==0)
                    {
                        std::cout << "Repeating <%s>\n"_f % history[a];
                        cmd = history[a];
                        break;
                    }
                if(cmd[0] == '!') std::cout << "No match found for (%s) from command history.\n"_f % cmd.substr(1);
                if(cmd[0] == '!') continue;
            }

            // Apply command aliases after dealing with the history
            for(;;)
            {
                std::string orig_cmd = cmd;
                for(const auto& r: aliases)
                    cmd = std::regex_replace(cmd, r.pattern, r.replacement);
                if(cmd == orig_cmd) break;
            }
            return cmd;
        }
    }
    void PrintHistory()
    {
        // Produce out the history of commands:
        std::cout << "Your latest commands of at least %d characters:\n"_f % int(HistMin);
        for(std::size_t a=0; a<history.size(); ++a)
            std::cout << "%3d : %s\n"_f % (a+1) % history[a];
    }
};

int main()
{
    std::cout << "Welcome to the treasure dungeon.\n\n";

    CommandReader cmd;
help:
    std::cout <<
        "Available commands:\n"
        "\tl/look\n"
        "\tn/s/w/e for moving\n"
        "\tquit\n"
        "\thelp\n\n"
        "You are starving. You are trying to find enough stuff to sell\n"
        "for food before you die. Beware, food is very expensive here.\n\n";

    // The main loop.
    Look();
    while(life > 0)
    {
        cmd.SetPrompt( "[life:%ld]> "_f % life );

        auto s = cmd.ReadCommand();
        // Produce the prompt and wait for player's command.
        if(s == "quit") break;
        if(s.empty()) continue;

        // Parse the command using C++11 regex.
        std::smatch res;

        #define rm std::regex_match

        // First, some metacommands
        if(s == "!?" || s == "history")     cmd.PrintHistory();
        else if(rm(s, R"((?:help|what|\?))"_r)) goto help;

        // Some fundamental movement commands
        else if(rm(s, "((go|walk|move) +)?(n|north)"_r)) { if(TryMoveBy( 0,-1)) Look(); }
        else if(rm(s, "((go|walk|move) +)?(s|south)"_r)) { if(TryMoveBy( 0, 1)) Look(); }
        else if(rm(s, "((go|walk|move) +)?(w|west)"_r))  { if(TryMoveBy(-1, 0)) Look(); }
        else if(rm(s, "((go|walk|move) +)?(e|east)"_r))  { if(TryMoveBy( 1, 0)) Look(); }
        else if(rm(s, "((go|walk|move) +)?(nw|northwest)"_r)) { if(TryMoveBy(-1,-1)) Look(); }
        else if(rm(s, "((go|walk|move) +)?(ne|northeast)"_r)) { if(TryMoveBy( 1,-1)) Look(); }
        else if(rm(s, "((go|walk|move) +)?(sw|southwest)"_r)) { if(TryMoveBy(-1, 1)) Look(); }
        else if(rm(s, "((go|walk|move) +)?(se|southeast)"_r)) { if(TryMoveBy( 1, 1)) Look(); }

        // Then commands for looking at things.
        // Use the power of regex to recognize complex syntax.
        else if(rm(s, "look( +around)?"_r)) Look();

        else if(rm(s, R"((?:wear|wield|eq)\b.*)"_r))
            std::cout << "You are scavenging for survival and not playing an RPG character.\n";
        else if(rm(s, R"(eat\b.*)"_r))
            std::cout << "You have nothing edible! You are hoping to collect something you can sell for food.\n";

        // Any unrecognized command.
        else std::cout << "what?\n";
    }

    std::cout
        << "%s\n"_f % (life<0
            ? "You are pulled out from the maze by a supernatural force!"
            : "byebye")
        << "[life:%ld] Game over\n"_f % life;
}



