// Architectures and Performance: Basic Benchmarking  Adam Sampson <a.sampson@abertay.ac.uk>
// Adapted to measure Dijkstras Algorithim by Corey Forbes

#include <chrono>
#include <iostream>
#include <thread>
#include <list>
#include <cstdlib>
#include <fstream>

// Import things we need from the standard library
using std::list;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::cout;
using std::endl;
using std::this_thread::sleep_for;
using std::ofstream;
// Define the alias "the_clock" for the clock type we're going to use.
// (You can change this to make the code below use a different clock.)
typedef std::chrono::steady_clock the_clock;

// The size of the array
#define WIDTH 64
#define HEIGHT 64

struct coord
{
	int y,
		x;
};

struct vertex
{
	int dist;
	coord prev;
};

struct Item
{
	int time;
	bool path;
};

bool operator==(const coord left, const coord right)
{
	return (left.y == right.y) && (right.x == left.x);
}
bool operator!=(const coord left, const coord right)
{
	return (left.y != right.y) && (right.x != left.x);
}

vertex array[HEIGHT][WIDTH];

void makeGraph(coord starting, coord target)
{
	// new method to fill array for 1/3 obsticals
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			if ((rand() % 3) == 1)
				array[y][x].dist = -2;
			else
				array[y][x].dist = HEIGHT + WIDTH; // * 10
		}
	}

	//Set start
	array[starting.y][starting.x].dist = 0;

	//Set end 
	array[target.y][target.x].dist = HEIGHT + WIDTH; // * 10

}

void displayGraph()
{
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			printf("%3d ", array[y][x].dist); // adapted to allow for better spacing and consistency on display
		}
		cout << endl;
	}

	return;
}

void displayPath(coord starting, coord target)
{
	coord currentCoord = target;
	int current = 1;
	cout << endl << "Path taken: " << endl;
	while (currentCoord != starting)
	{
		cout << "X" << current << " = " << currentCoord.x << " - Y" << current << " = " << currentCoord.y << endl;
		currentCoord = array[currentCoord.y][currentCoord.x].prev;
		current++;
	}
}

bool locateEnd(coord current, int loop, coord &target)
{
	bool found = false;
	for (int y = -1; y < 2; y++) // checking 3x3 on the Y axis
	{
		for (int x = -1; x < 2; x++) // checking 3x3 on the X axis
		{
			if ((current.y + y) < HEIGHT && (current.y + y) >= 0 
				&& (current.x + x) < WIDTH && (current.x + x) >= 0) //if point is valid and not circling through the array
			{
				if ((current.y + y == target.y) && (current.x + x == target.x)) //if found the end point
				{
					found = true;
				}
				if (array[(current.y + y)][(current.x + x)].dist == (HEIGHT + WIDTH)) // * 10 if not obstacle or prev visit
				{
					array[(current.y + y)][(current.x + x)].dist = loop + 1; // flood
					array[(current.y + y)][(current.x + x)].prev.x = current.x;
					array[(current.y + y)][(current.x + x)].prev.y = current.y;
				}

			}
		}
	}
	return found;
}

bool explore(int &loop, coord &target)
{
	bool found = false;
	//Explore possible paths
	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			if (array[y][x].dist == loop)
			{
				found = locateEnd(coord{ y, x }, loop, target);
				
				if (found)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool floodGraph(coord &target)
{
	bool found = false;
	int loop = 0;

	while (!found)
	{
		//system("CLS");
		//displayGraph(); //Show current graph

		found = explore(loop, target);
			
		//sleep_for(milliseconds(100));
		loop++;

		if (loop > HEIGHT + WIDTH) // * 10
			return false;
	}
	return true;
}

void dijk(Item &result)
{
	bool sucsess;
	coord starting{ 1, 1 }; // will always start one in from the top left on both axies
	coord target{ HEIGHT - 2, WIDTH - 2 }; // will aways be one in from the bottom right on both axies

	makeGraph(starting, target); // make a graph with obstacles

	// Start timing
	the_clock::time_point start = the_clock::now();
	
	// Do something
	sucsess = floodGraph(target);// flood the graph until valid path

	//system("CLS"); // clear

	if (sucsess)
	{
		//displayGraph(); // show a final graph
		displayPath(starting, target); // show the coords visited
	}
	else // if no path is found or the path would be longer than HEIGHT + WIDTH * 10 which it shouldnt ever
	{ cout << "A valid path is unavailible" << endl; }

	// Stop timing
	the_clock::time_point end = the_clock::now();
	// Compute the difference between the two times in milliseconds
	auto time_taken = duration_cast<milliseconds>(end - start).count();
	
	result.time = time_taken;
	result.path = sucsess;

	return;
}

void main()
{
	srand(time(NULL));
	Item current{ 0, 0 };

	ofstream myResults;
	myResults.open("results.txt");

	myResults << "Time - Path\n";
	for (size_t i = 0; i < 1000; i++)
	{
		dijk(current);
		myResults << current.time << "	" << current.path << "\n";
		sleep_for(milliseconds(50));
	}
	myResults.close();
	return;
}

/*
function Dijkstra(source)
{
create vertex set Q

for each vertex v in Graph: // Initialization
dist[v] ← INFINITY // Unknown distance from source to v
prev[v] ← UNDEFINED // Previous node in optimal path from source
add v to Q // All nodes initially in Q (unvisited nodes)

dist[source] ← 0 // Distance from source to source

while Q is not empty:
u ← vertex in Q with min dist[u] //Source node first selected
remove u from Q

for each neighbor v of u: // where v is still in Q.
alt ← dist[u] + length(u, v)
if alt < dist[v]: // A shorter path to v has been found
dist[v] ← alt
prev[v] ← u

return dist[], prev[]
}
*/