#include<string>
#include <type_traits>

#define GR_H_S 16
#define GR_H_M 32
#define GR_H_L 64

#define GR_W_S 32
#define GR_W_M 64
#define GR_W_L 128

typedef char (*GraphRefS)[GR_H_S][GR_W_S];
typedef char (*GraphRefM)[GR_H_M][GR_W_M];
typedef char (*GraphRefL)[GR_H_L][GR_W_L];

template <class T>
void updatePriv(T curr, T next, int offsetX, int offsetY, int height, int width)
{
    for (int i = height - 1; i > -1; i--) // draws from top to bottom
    {
      for (int j = 0; j < width; j++)
      {
        if ((*curr)[i][j] != (*next)[i][j])
        {
          (*curr)[i][j] = (*next)[i][j];
          printf("\033[%d;%dH", height - i + offsetY, j + offsetX); // move to char location
          printf("%c", (*curr)[i][j]); // update char on screen
        }
      }
    }
};

template <typename T>
class GraphRef {
private:
    T curr;
    T next;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;

    GraphRef(T curr, T next, int offsetX, int offsetY)
      : curr(curr)
      , next(next)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height{5}
      , width{5}
      {};
    
    void update() {};
};

template <>
class GraphRef<GraphRefS> {
private:
    GraphRefS curr;
    GraphRefS next;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;

    GraphRef(string name, GraphRefS curr, GraphRefS next, int offsetX, int offsetY)
      : name(name)
      , curr(curr)
      , next(next)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_S)
      , width(GR_W_S) 
      {};

    void update()
    {
      updatePriv<GraphRefS>(curr, next, offsetX, offsetY, height, width);
    };
};

template <>
class GraphRef<GraphRefM> {
private:
    GraphRefM curr;
    GraphRefM next;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;
    GraphRef(string name, GraphRefM curr, GraphRefM next, int offsetX, int offsetY)
      : name(name)
      , curr(curr)
      , next(next)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_M)
      , width(GR_W_M) 
      {};

    void update()
    {
      updatePriv<GraphRefM>(curr, next, offsetX, offsetY, height, width);
    };
};

template <>
class GraphRef<GraphRefL> {
private:
    GraphRefL curr;
    GraphRefL next;
    int offsetX;
    int offsetY;
    int height;
    int width;

public:
    string name;
    GraphRef(string name, GraphRefL curr, GraphRefL next, int offsetX, int offsetY)
      : name(name)
      , curr(curr)
      , next(next)
      , offsetX(offsetX)
      , offsetY(offsetY)
      , height(GR_H_L)
      , width(GR_W_L) 
      {};

    void update()
    {
      updatePriv<GraphRefL>(curr, next, offsetX, offsetY, height, width);
    };
};
