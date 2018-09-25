// Architectures and Performance: Basic Benchmarking  Adam Sampson <a.sampson@abertay.ac.uk>
// Adapted to measure Lee Algorithim by Corey Forbes

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

int array[HEIGHT][WIDTH];

struct Coord
{
	int y,
		x;
};

struct Item
{
	int time;
	bool path;
};

void displayGraph()
{
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			printf("%3d ", array[y][x]); // to allow for spacing and consistency on displays
		}
		cout << endl;
	}

	return;
}

void displayPath(list<Coord> &returnPath)
{
	int current = 0;
	Coord temp;
	cout << endl << "Path taken: " << endl;
	while (!returnPath.empty())
	{
		temp = returnPath.back();
		cout << "X" << current << " = " << temp.x << " - Y" << current << " = " << temp.y << endl;
		returnPath.pop_back();
		current++;
	}
}

void makeGraph(Coord starting, Coord target)
{
	// new method to fill array for half obsticals
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			if ((rand() % 3) == 1)
				array[y][x] = -2;
			else
				array[y][x] = -1;
		}
	}

	//Set start
	array[starting.y][starting.x] = 0;

	//Set end 
	array[target.y][target.x] = -3;

}

bool locateEnd(int y, int x, int loop)
{
	bool found = false;
	for (int column = -1; column < 2; column++) // checking 3x3 on the Y axis
	{
		for (int row = -1; row < 2; row++) // checking 3x3 on the X axis
		{
			if ((y + column) < HEIGHT && (y + column) >= 0 && (x + row) < WIDTH && (x + row) >= 0) //if point is valid and not circling through the array
			{
				if (array[(y + column)][(x + row)] == -3) //if found the end point
				{
					found = true;
				}
				if (array[(y + column)][(x + row)] == -1) // if not obstacle and less than 0
				{
					array[(y + column)][(x + row)] = loop + 1; // flood
				}

			}
		}
	}
	return found;
}

bool locatePath(int loop, Coord &current)
{
	bool found = false;
	for (int column = -1; column < 2; column++) // checking 3x3 on the Y axis
	{
		for (int row = -1; row < 2; row++) // checking 3x3 on the X axis
		{
			if ((current.y + column) < HEIGHT && (current.y + column) >= 0 && (current.x + row) < WIDTH && (current.x + row) >= 0) //if point is valid and not circling through the array
			{
				if (array[(current.y + column)][(current.x + row)] == loop) // if equal to 1 less than current
				{
					array[(current.y + column)][(current.x + row)] = -3;
					current = { current.y + column, current.x + row };
					column = 2; row = 2; //break loop
				}
			}
		}
	}

	return found;
}

bool explore(int loop, Coord target)
{
	bool found = false;
	//Explore possible paths
	for (int y = 0; y < HEIGHT; ++y)
	{
		for (int x = 0; x < WIDTH; ++x)
		{
			if (array[y][x] == loop)
			{
				found = locateEnd(y, x, loop);
				
				if (found)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool floodGraph(int &loop, Coord target)
{
	bool found = false;

	while (!found)
	{
		//system("CLS");
		//displayGraph(); //Show current graph

		found = explore(loop, target);
			
		//sleep_for(milliseconds(100));
		loop++;

		if (loop > HEIGHT + WIDTH)
			return false;
	}
	return true;
}

void showPath(int loop, Coord starting, Coord target, list<Coord> &returnPath)
{
	Coord current = target; // push the first Coord on the way back
	
	while (loop > 0)
	{
		locatePath(loop, current);
		returnPath.push_back(current);
		//displayGraph();

		//sleep_for(milliseconds(100));
		
		//system("CLS");
		loop--;
	}
}

void lee(Item &result)
{
	bool sucsess;
	int loop = 0;
	Coord starting{ 1, 1 }; // will always start one in from the top left on both axies
	Coord target{ HEIGHT - 2, WIDTH - 2 }; // will aways be one in from the bottom right on both axies
	list<Coord>returnPath;
	
	// Do something
	makeGraph(starting, target); // make a graph with obstacles
	
	// Start timing
	the_clock::time_point start = the_clock::now();

	sucsess = floodGraph(loop, target);// flood the graph until valid path
	
	//system("CLS"); // clear

	if (sucsess)
	{
		showPath(loop, starting, target, returnPath); // find the quickest path

		//displayGraph(); // show a final graph

		displayPath(returnPath); // show the coords visited
	}
	else // if no path is found or the path would be longer than HEIGHT + WIDTH which it shouldnt ever
	{
		cout << "A valid path is unavailible" << endl;
	}
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
		lee(current);
		myResults << current.time << "	" << current.path << "\n";
		sleep_for(milliseconds(50));
	}
	myResults.close();
	return;
}
