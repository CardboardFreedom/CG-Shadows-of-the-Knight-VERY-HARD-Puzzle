//

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

class coord
{
public:
    coord(){}
    coord(int a, int b) : x(a), y(b) {}

    int x = -1, y = -1;

    void update(int one, int two) { x = one; y = two; }
	int getY() { return y; }
	int getX() { return x; }
    bool operator!=(coord one) { return (one.x != x || one.y != y); }
};

double pow2(double one) { return one * one; }

double cut(coord prev, coord curr, int X)
{	//curr							  prev
	//(x2 - x1)^2  +  (y2 - y1)^2  =  (x2 - x1)^2  +  (y2 - y1)^2

	double y1 = -curr.y;
	double left = pow2(X - curr.x);
	double foiled = pow2(y1);
	double numY = y1 * 2;

	left += foiled;
	y1 = -prev.y;

	double right = pow2(X - prev.x);
	double foiledR = pow2(y1);
	double numYR = y1 * 2;

	right += foiledR;
	numY -= numYR;
	right -= left;

	return right / numY;
}

int main()
{
    int W; // width of the building.
    int H; // height of the building.
    cin >> W >> H; cin.ignore();
    int N; // maximum number of turns before game over.
    cin >> N; cin.ignore();
    int X0;
    int Y0;
    cin >> X0 >> Y0; cin.ignore();

    int iS = 0;
    int windows = W*H;

    vector<vector<bool>> map(W, vector<bool>(H,true));
    vector<int> jS(H, 0);

    coord current(X0, Y0);
    coord prev;

    // game loop
    while (1)
    {
        string bombDir; // Current distance to the bomb compared to previous distance (COLDER, WARMER, SAME or UNKNOWN)
        cin >> bombDir;

        bool w = false, c = false, s = false;

        if(bombDir == "UNKNOWN")
        {
            prev = current;
            current = coord(W/2, H/2);
            cout << W/2 << ' ' << H/2 << '\n';
            map[W/2][H/2] = false;
            windows--;
            N--;
            continue;
        }

        if(bombDir == "WARMER")w = true;
        else if(bombDir == "COLDER")c = true;
        else if(bombDir == "SAME")s = true;

        coord Temp;
        coord P1(-1, -1), P2(-1, -1), P3(-1, -1), P4(-1, -1);

		double position = cut(prev, current, current.getX());
		bool nInf = 0, pInf = 0, above = 0, under = 0;

		if (isinf(position))
		{
			if (position == std::numeric_limits<double>::infinity())
			    pInf = true;
                
			else nInf = true;
		}

		else
		{
			if (position > current.getY()) under = true;
			if (position < current.getY()) above = true;
		}

        windows = 0;
        for(int i = iS; i < map.size(); i++)
        {
            double d = cut(prev, current, i);

			if (bombDir == "WARMER")
			{
				if (above)
				{
					d = int(d) + 1;
					if(jS[i] < d) jS[i] = d;
				}

				else if (under && map[i].size() > d)
				{
					d = ceil(d);

					if (d < 0) map[i].clear();
					else map[i].resize(ceil(d));
				}

				else if (pInf)
				{
					if (d != std::numeric_limits<double>::infinity())
						map[i].clear();
				}

				else if (nInf)
				{
					if (d != -std::numeric_limits<double>::infinity())
						map[i].clear();
				}
			}

			if (bombDir == "COLDER")
			{
				if (above && map[i].size() > d)
				{
					d = ceil(d);
					if (d < 0) map[i].clear();
					else map[i].resize(ceil(d));
				}

				else if (under)
				{
					d = int(d) + 1;
					if (jS[i] < d) jS[i] = d;
				}

				else if (pInf)
				{
					if (d != -std::numeric_limits<double>::infinity())
						map[i].clear();
				}

				else if (nInf)
				{
					if (d != std::numeric_limits<double>::infinity())
						map[i].clear();
				}
			}

			if (bombDir == "SAME" && prev != current)
			{
				if (!(d != d))
				{
					map[i] = vector<bool>(map[i].size(), 0);

					if (!isinf(d) && int(d) < map[i].size())
						map[i][int(d)] = 1;

					jS[i] = d;
				}
			}

            if(map[i].size() > 0 && jS[i] < map[i].size())
            {
                int aq = map[i].size()-1;
                while(!map[i][aq] && aq > jS[i])aq--;

                if(map[i][aq])
                    Temp = coord(i, aq);
                    
                if(P1.x == -1)
                {
                    P1.x = i;
                    P1.y = jS[i];
                }

                if(i == P1.x)
                {
                    P2.x = i;
                    P2.y = map[i].size()-1;
                }

                if(i != P3.x)
                {
                    P3.x = i;
                    P3.y = jS[i];
                }

                if(i == P3.x)
                {
                    P4.x = i;
                    P4.y = map[i].size()-1;
                }

                windows += map[i].size() - jS[i];
            }
        }

        cerr << "\nWindows: " << windows << '\n';
        if(windows < N)
        {
            prev = current;
            current = Temp;
            cout << Temp.x << ' ' << Temp.y << '\n';
            map[Temp.x][Temp.y] = false;
            windows--;
            N--;
            continue;
        }

        if(windows < 50)
        {
            vector<coord> win;
            vector<int> distances;

            vector<pair<coord, int>> options;

            for(int i = 0; i < map.size(); i++)
                for(int j = jS[i]; j < map[i].size(); j++)
                    if(map[i][j])
                    {
                        win.push_back(coord(i, j));
                        distances.push_back(sqrt( pow2(current.x - i) + pow2(j - current.y) ));
                    }
                    
            for(auto &i:win)
            {
                int f = 0, c = 0;
                auto currDistance = distances.begin();

                for(auto &j:win)
                {
                    if(&i == &j)
                    {
                        currDistance++;
                        continue;
                    }

                    int d = sqrt( pow2(j.x - i.x) + pow2(i.y - j.y) );

                    if(d < *currDistance)
                        c++;
                    else if(d > *currDistance)
                        f++;

                    currDistance++;
                }

                options.push_back({i, abs(f-c)});
            }

            sort(options.begin(), options.end(), [](auto one, auto two)
            {
                return one.second < two.second;
            });

            cout << options[0].first.x << ' ' << options[0].first.y << '\n';
            prev = current;
            current = options[0].first;
            if(map[options[0].first.x][options[0].first.y]) windows--;
            map[options[0].first.x][options[0].first.y] = false;
            N--;
            continue;
        }

        bool size = true;

        if(size)
        {
            coord temp(-1, -1);
            
            coord *lowX = nullptr;
            coord *highX = nullptr;

            coord *lowY = nullptr;
            coord *highY = nullptr;

            lowX = (P1.x > P2.x ? &P2:&P1);
            lowX = (lowX->x > P3.x ? &P3:lowX);
            lowX = (lowX->x > P4.x ? &P4:lowX);
            
            highX = (P1.x < P2.x ? &P2:&P1);
            highX = (highX->x < P3.x ? &P3:highX);
            highX = (highX->x < P4.x ? &P4:highX);

            int x = highX->x - lowX->x;

            lowY = (P1.y > P2.y ? &P2:&P1);
            lowY = (lowY->y > P3.y ? &P3:lowY);
            lowY = (lowY->y > P4.y ? &P4:lowY);
            
            highY = (P1.y < P2.y ? &P2:&P1);
            highY = (highY->y < P3.y ? &P3:highY);
            highY = (highY->y < P4.y ? &P4:highY);

            int y = highY->y - lowY->y;

            coord temp1, temp2;

            if(true)//x > y
            {
                double distance = abs(current.x - ((highX->x + lowX->x) / 2));

                temp.x = (highX->x + lowX->x) / 2;
                temp.y = current.y;

                if(current.x < (highX->x + lowX->x) / 2)
                    temp.x += distance;
                else temp.x -= distance;

                if(!(temp.x >= 0 && temp.x < W) || !map[temp.x][temp.y])
                    temp.x--;
                if(!(temp.x >= 0 && temp.x < W) || !map[temp.x][temp.y])
                    temp.x += 2;

                while(temp.x >= W) temp.x--;
                while(temp.x < 0) temp.x++;

                temp1 = temp;

            }
            if(true)
            {
                double distance = abs(current.y - ((highY->y + lowY->y) / 2));

                temp.x = current.x;
                temp.y = (highY->y + lowY->y) / 2;

                if(current.y < (highY->y + lowY->y) / 2)
                    temp.y += distance;
                else temp.y -= distance;

            
                while(temp.y >= H) temp.y--;
                while(temp.y < 0) temp.y++;

                if(temp.y > 0 && !map[temp.x][temp.y])
                    temp.y--;
                if(temp.y+2 < H && !map[temp.x][temp.y])
                    temp.y += 2;

                temp2 = temp;
            }

            if((x > y && map[temp1.x][temp1.y]) || !map[temp2.x][temp2.y])
                temp = temp1;
            else temp = temp2;

            if(temp.y > map[temp.x].size()-1 || !map[temp.x][temp.y])
            {
                temp.x = (highX->x + lowX->x) / 2;
                temp.y = (highY->y + lowY->y) / 2;
            }

            if(temp.y > map[temp.x].size()-1 || !map[temp.x][temp.y])
            {
                temp.x = lowX->x;
                temp.y = (highY->y + lowY->y) / 2;
            }

            if(!map[temp.x][temp.y])
            {
                temp.x = (highX->x + lowX->x) / 2;
                temp.y = lowY->y;
            }
            
            cout << temp.x << ' ' << temp.y << '\n';
            prev = current;
            current = temp;
            if(map[temp.x][temp.y]) windows--;;
            map[temp.x][temp.y] = false;
            N--;
            continue;
        }
    }
}
