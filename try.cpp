#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <utility>
#include <algorithm>

using namespace std;

struct SquareGrid
{
  vector<pair<int, int>> direction{make_pair(1, 0), make_pair(0, -1), make_pair(-1, 0), make_pair(0, 1)};
  vector<vector<char>> grid;
  map<char, pair<int, int>> c2p;
  map<pair<int, int>, char> p2c;

  SquareGrid(vector<vector<char>> grid_, map<char, pair<int, int>> c2p_, map<pair<int, int>, char> p2c_) : grid(grid_), c2p(c2p_), p2c(p2c_) {}

  int grid_size = grid.size();

  bool in_bounds(pair<int, int> id)
  {
    return 0 <= id.first && id.first < grid_size && 0 <= id.second && id.second < grid_size;
  }

  bool passable(pair<int, int> id)
  {
    return grid[id.first][id.second] != '#';
  }

  vector<char> neighbors(char key)
  {
    vector<char> results;
    pair<int, int> id = c2p[key];

    for (pair<int, int> dir : direction)
    {
      pair<int, int> next = make_pair(id.first + dir.first, id.second + dir.second);

      if (in_bounds(next) && passable(next))
      {
        char c_next = p2c[{next.first, next.second}];
        results.push_back(c_next);
      }
    }

    if ((id.first + id.second) % 2 == 0)
      reverse(results.begin(), results.end());

    return results;
  }
};

char choice()
{
  char choice;
  cout << "=================================" << endl;
  cout << "|      Choose Option Below      |" << endl;
  cout << "=================================" << endl;
  cout << "|    1. Breadth First Search    |" << endl;
  cout << "|    2. Uniform Cost Search     |" << endl;
  cout << "|    3. A* Search               |" << endl;
  cout << "|    4. View Given Grid         |" << endl;
  cout << "|    5. Exit                    |" << endl;
  cout << "=================================" << endl;

  while (1)
  {
    cout << "Enter your choice: ";
    cin >> choice;
    if (choice != '1' && choice != '2' && choice != '3' && choice != '4' && choice != '5')
      cout << "Wrong keyword!" << endl;
    else
      break;
  }

  return choice;
}

void print(SquareGrid graph)
{
  for (int i = 0; i < graph.grid.size(); ++i)
  {
    for (int j = 0; j < graph.grid[i].size(); ++j)
      cout << graph.grid[i][j];
    cout << endl;
  }
}

void path(SquareGrid graph, map<char, char> came_from, char start, char goal)
{
  char current = goal;
  vector<char> path;

  while (current != start)
  {
    current = came_from[current];
    path.push_back(current);
  }

  for (int i = 0; i < path.size(); ++i)
  {
    pair<int, int> p = graph.c2p[path[i]];
    if (graph.grid[p.first][p.second] == '-')
      graph.grid[p.first][p.second] = 'p';
  }

  print(graph);
}

class compare
{
public:
  bool operator()(pair<char, int> a, pair<char, int> b)
  {
    return a.second > b.second;
  }
};

int heuristic(SquareGrid graph, char a, char b)
{
  return abs(graph.c2p[a].first - graph.c2p[b].first) + abs(graph.c2p[a].second - graph.c2p[b].second);
}

void Breadth_First_Search(SquareGrid graph, char start, char goal)
{
  queue<char> frontier;
  frontier.push(start);
  map<char, char> came_from;
  came_from[start] = start;

  while (!frontier.empty())
  {
    char current = frontier.front();
    frontier.pop();

    if (current == goal)
      break;

    for (char next : graph.neighbors(current))
    {
      if (came_from.find(next) == came_from.end())
      {
        frontier.push(next);
        came_from[next] = current;
      }
    }
  }

  path(graph, came_from, start, goal);
}

void Uniform_Cost_Search(SquareGrid graph, char start, char goal)
{
  priority_queue<pair<char, int>, vector<pair<char, int>>, compare> frontier;
  map<char, char> came_from;
  map<char, int> cost_so_far;

  frontier.push(make_pair(start, 0));
  came_from[start] = start;
  cost_so_far[start] = 0;

  while (!frontier.empty())
  {
    char current = frontier.top().first;
    frontier.pop();

    if (current == goal)
      break;

    for (char next : graph.neighbors(current))
    {
      int new_cost = cost_so_far[current] + 1;
      if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next])
      {
        cost_so_far[next] = new_cost;
        came_from[next] = current;
        frontier.push(make_pair(next, new_cost));
      }
    }
  }
  path(graph, came_from, start, goal);
}

void A_Star_Search(SquareGrid graph, char start, char goal)
{
  priority_queue<pair<char, int>, vector<pair<char, int>>, compare> frontier;
  map<char, char> came_from;
  map<char, int> cost_so_far;

  frontier.push(make_pair(start, heuristic(graph, start, goal)));
  came_from[start] = start;
  cost_so_far[start] = 0;

  while (!frontier.empty())
  {
    char current = frontier.top().first;
    frontier.pop();

    if (current == goal)
      break;

    for (char next : graph.neighbors(current))
    {
      int new_cost = cost_so_far[current] + 1;
      if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next])
      {
        cost_so_far[next] = new_cost;
        int priority = new_cost + heuristic(graph, next, goal);
        came_from[next] = current;
        frontier.push(make_pair(next, priority));
      }
    }
  }
  path(graph, came_from, start, goal);
}

int main()
{
  int grid_size, number_of_obstacles, key_value = 65, s_row, s_column, t_row, t_column;

  cout << "Enter NxN grid size: ";
  cin >> grid_size;

  map<char, pair<int, int>> c2p;
  map<pair<int, int>, char> p2c;
  vector<vector<char>> grid(grid_size, vector<char>(grid_size, '-'));

  for (int i = 0; i < grid_size; ++i)
  {
    for (int j = 0; j < grid_size; ++j)
    {
      char key = key_value++;
      c2p[key] = make_pair(i, j);
      p2c[{i, j}] = key;
    }
  }

  cout << "Enter the number of total obstacles: ";
  cin >> number_of_obstacles;

  for (int i = 0; i < number_of_obstacles; ++i)
  {
    cout << "Enter the position of obstacles no_" << i + 1 << ": ";
    cin >> s_row >> s_column;
    grid[s_row][s_column] = '#';
  }

  cout << "Enter the position of robot: ";
  cin >> s_row >> s_column;
  grid[s_row][s_column] = 's';

  cout << "Enter the position of target: ";
  cin >> t_row >> t_column;
  grid[t_row][t_column] = 't';

  SquareGrid graph(grid, c2p, p2c);
  char start = graph.p2c[{s_row, s_column}];
  char goal = graph.p2c[{t_row, t_column}];

  while (1)
  {
    switch (choice())
    {
    case '1':
      Breadth_First_Search(graph, start, goal);
      break;
    case '2':
      Uniform_Cost_Search(graph, start, goal);
      break;
    case '3':
      A_Star_Search(graph, start, goal);
      break;
    case '4':
      print(graph);
      break;
    case '5':
      return 0;
    }
  }
}