#include <iostream>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <chrono>
#include <assert.h>
#include <map>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <windows.h>
#include <conio.h>
#include <algorithm>
#include <functional>
#include <queue>
using namespace std;

// 所有位置存储都是1，只有绘制的时候，x *= 2 y *= 1
class Console
{
public:
    enum Color { COLOR_NO = 0, COLOR_WHITE, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_PINK, COLOR_CYAN, COLOR_MAX };
    enum Key { KEY_NO, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE, KEY_MAX };

    static void setColor(Color color)
    {
        static int color_arr[COLOR_MAX] =
        {
            0,
            FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
            FOREGROUND_INTENSITY | FOREGROUND_RED,
            FOREGROUND_INTENSITY | FOREGROUND_GREEN,
            FOREGROUND_INTENSITY | FOREGROUND_BLUE,
            FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
            FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
            FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE
        };

        ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), color_arr[color]);
    }

    static void setPos(int i, int j)			//控制光标位置， 列， 行
    {
        COORD pos = { i,j };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }

    static void setCursorVisible(bool visible)
    {
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO CursorInfo;
        GetConsoleCursorInfo(handle, &CursorInfo);//获取控制台光标信息
        CursorInfo.bVisible = visible; //隐藏控制台光标
        SetConsoleCursorInfo(handle, &CursorInfo);//设置控制台光标状态
    }

    static Key getKey()
    {
        Key retkey = KEY_NO;

        if (_kbhit())				//键盘输入值时 
        {
            int key, key2;
            key = _getch();
            if (key == 224)
            {
                key2 = _getch();

                switch (key2)
                {
                case 72: retkey = KEY_UP; break; //按向上方向键时
                case 80: retkey = KEY_DOWN; break; //按向下方向键时
                case 75: retkey = KEY_LEFT; break; //按向左方向键时
                case 77: retkey = KEY_RIGHT; break; //按向右方向键时
                default:
                    break;
                }
            }
            else if (key == 32)					// 按下空格暂停
            {
                retkey = KEY_SPACE;
            }
        }

        return retkey;
    }
};

// 每个组件都是Block 包括墙和pane
class Block
{
public:
    Block(int h = 3, int w = 3)
        :_locX(0), _locY(0), _status(0), _parent(nullptr)
    {
        // size: 0, 0
        setSize(h, w);
    }

    void setParent(Block* parent)
    {
        _parent = parent;
    }

    // 获取 相对于 other 的相对坐标
    void getRelativeLoc(const Block& other, int& x, int& y) const 
    {
        int gx1, gy1, gx2, gy2;

        getAbsoluteLoc(gx1, gy1);
        other.getAbsoluteLoc(gx2, gy2);

        x = gx1 - gx2;
        y = gy1 - gy2;
    }

    void getAbsoluteLoc(int& gx, int& gy) const
    {
        gx = _locX;
        gy = _locY;

        Block* parent = _parent;

        while (parent)
        {
            gx += parent->_locX;
            gy += parent->_locY;
            parent = parent->_parent;
        }
    }

    void setSize(int w, int h)
    {
        _shape.clear();
        for (int i = 0; i < h; i++)
            _shape.emplace_back(vector<int>(w));
    }

    void setLoc(int x, int y)
    {
        _locX = x, _locY = y;
    }

    virtual void genShape() { /* empty shape */ }
    virtual void turn()
    {
        this->rotate();
    }
    virtual Block* clone() { return nullptr; }

    // 判断block是否为后代
    bool isDescendant(Block& block)
    {
        bool isChild = false;
        Block* parent = block._parent;
        while (parent)
        {
            if (parent == this)
            {
                isChild = true;
                break;
            }

            parent = parent->_parent;
        }

        return isChild;
    }

    // 判断一个子组件是否和宿主组件重叠
    bool isconflict(Block& child)
    {
        assert(isDescendant(child));

        if (_shape.size() == 0 || _shape[0].size() == 0)
            return false;

        int rxChild, ryChild; // 获取 child 相对于当前块的相对坐标
        child.getRelativeLoc(*this, rxChild, ryChild);

        for (int r = 0; r < child._shape.size(); r++)
        {
            for (int c = 0; c < child._shape[r].size(); c++)
            {
                int gx1 = rxChild + c; // 
                int gy1 = ryChild + r; //

                if (gx1 >= 0 && gx1 < _shape[0].size() && gy1 >= 0 && gy1 < _shape.size())
                {
                    if (child._shape[r][c] && _shape[gy1][gx1])
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    // merge other
    void merge(const Block& other)
    {
        if (_shape.size() == 0 || _shape[0].size() == 0)
            return;

        int rxOther, ryOther; // 获取 other 相对于当前块的相对坐标
        other.getRelativeLoc(*this, rxOther, ryOther);

        for (int r = 0; r < other._shape.size(); r++)
        {
            for (int c = 0; c < other._shape[r].size(); c++)
            {
                int gx1 = rxOther + c;
                int gy1 = ryOther + r;

                if (gy1 >= 0 && gy1 < _shape.size() && gx1 >= 0 && gx1 < _shape[0].size())
                    if (other._shape[r][c]) //merge valid 
                        _shape[gy1][gx1] = other._shape[r][c];
            }
        }
    }

    void genColor()
    {
        std::vector<Console::Color> colors =
        { Console::COLOR_RED, Console::COLOR_GREEN, Console::COLOR_BLUE, 
            Console::COLOR_YELLOW, Console::COLOR_PINK, Console::COLOR_CYAN };

        std::random_shuffle(colors.begin(), colors.end());

        int idx = 0;

        for (int i = 0; i < _shape.size(); i++)
        {
            for (int j = 0; j < _shape[i].size(); j++)
            {
                if (_shape[i][j])
                    _shape[i][j] = colors[idx++ % colors.size()];
            }
        }
    }

    void show() const
    {
        for (int i = 0; i < _shape.size(); i++)
        {
            for (int j = 0; j < _shape[i].size(); j++)
            {
                cout << _shape[i][j] << " ";
            }
            cout << endl;
        }
    }

    virtual void paint() const
    {
        int gx, gy;
        getAbsoluteLoc(gx, gy);

        for (int i = 0; i < _shape.size(); i++)
        {
            for (int j = 0; j < _shape[i].size(); j++)
            {
                if (_shape[i][j] > 0)
                {
                    if (_parent)
                    {
                        int px, py;
                        getRelativeLoc(*_parent, px, py);
                        px += j;
                        py += i;
                        if (px < 0 || px >= _parent->Shape()[0].size() ||
                            py < 0 || py >= _parent->Shape().size())
                        {
                            continue;
                        }
                    }

                    Console::setPos((gx + j) * 2, gy + i);
                    Console::setColor((Console::Color)_shape[i][j]);
                    cout << "■";
                }
            }
        }
    }

    void repaint() const
    {
        int gx, gy;
        getAbsoluteLoc(gx, gy);

        for (int i = 0; i < _shape.size(); i++)
        {
            for (int j = 0; j < _shape[i].size(); j++)
            {
                if (_shape[i][j])
                {
                    Console::setPos((gx + j) * 2, gy + i);
                    cout << "  "; //两个空格
                }
            }
        }
    }

    // 顺时针旋转90°
    void rotate() {
        //矩阵转置代码
        for (int i = 0; i < _shape.size(); i++) {
            for (int j = i; j < _shape[i].size(); j++) {
                int k = _shape[i][j];
                _shape[i][j] = _shape[j][i];
                _shape[j][i] = k;
            }
        }
        //矩阵调换列元素
        int a = 0;
        int b = _shape.size() - 1;
        for (int i = 0; i < (_shape.size()) / 2; i++) {
            for (int j = 0; j < _shape.size(); j++) {
                int k = _shape[j][a];
                _shape[j][a] = _shape[j][b];
                _shape[j][b] = k;
            }
            a++;
            b--;
        }
    }

    //求最大公约数
    int gcd(int a, int b) {
        if (a == 0 || b == 0) {
            return a + b;
        }
        int t;
        while (b != 0) {
            t = a % b;
            a = b;
            b = t;
        }

        return a;
    }

    // n > 0 ->
    // n < 0 <-
    // down +/ up-

    void ShiftVertical(int n)
    {
        if (_shape.size() == 0)
            return;

        int size = _shape[0].size();
        n %= size;
        if (n == 0)
            return;

        int g = gcd(size, abs(n));

        for (int r = 0; r < _shape.size(); r++)
        {
            for (int i = 0, _i = g; i < _i; ++i)
            {
                int k = i;
                int t = _shape[r][k];
                do {
                    k = ((k + n) + size) % size;
                    int tt = _shape[r][k];
                    _shape[r][k] = t;
                    t = tt;
                } while (k != i);
            }
        }
    }

    void ShiftHorizontal(int n)
    {
        if (_shape.size() == 0)
            return;

        int size = _shape.size();
        n %= size;
        if (n == 0)
            return;

        int g = gcd(size, abs(n));

        for (int c = 0; c < _shape[0].size(); c++)
        {
            for (int i = 0, _i = g; i < _i; ++i)
            {
                int k = i;
                int t = _shape[k][c];
                do {
                    k = ((k + n) + size) % size;
                    int tt = _shape[k][c];
                    _shape[k][c] = t;
                    t = tt;
                } while (k != i);
            }
        }
    }

    // return the min not empty line index
    int validRow() const
    {
        int r = 0;
        for (r = 0; r < _shape.size(); r++)
        {
            for (int c = 0; c < _shape[r].size(); c++)
            {
                if (_shape[r][c])
                    return r;
            }
        }

        return r;
    }

    vector<vector<int>>& Shape() { return _shape; }

    int _locX, _locY; //相对父窗口偏移位置
    vector<vector<int>> _shape;
    int _status; // 0, 90, 180, 270
    Block* _parent;
};

class Wall : public Block
{
public:
    virtual void genShape() override
    {
        if (_shape.size() <= 0 || _shape[0].size() <= 0)
            return;
        //for (int i = 0; i < _shape[0].size(); i++)
        //    _shape[0][i] = Console::COLOR_WHITE;
        for (int i = 0; i < _shape[_shape.size() - 1].size(); i++)
            _shape[_shape.size() - 1][i] = Console::COLOR_WHITE;
        for (int i = 1; i < _shape.size() - 1; i++)
            _shape[i][0] = Console::COLOR_WHITE;
        for (int i = 1; i < _shape.size() - 1; i++)
            _shape[i][_shape[i].size() - 1] = Console::COLOR_WHITE;
    }

    virtual void paint() const
    {
        Block::paint();
        int gx, gy;
        getAbsoluteLoc(gx, gy);

        for (int i = 0; i < _shape[0].size(); i++)
        {
            Console::setPos((gx + i) * 2, gy);
            Console::setColor(Console::COLOR_WHITE);
            cout << "■";
        }
    }
};

class Pane : public Block
{
public:
    virtual void genShape() override
    {
        return; //empty
    }

    virtual Pane* clone() override { return new Pane(*this); }
};

// "■■■■"
class BlockA : public Block
{
public:
    BlockA() : Block(4, 4) {}
    virtual void genShape() override
    {
        for (int i = 0; i < _shape[0].size(); i++)
            _shape[1][i] = 1;
    }

    virtual BlockA* clone() override { return new BlockA(*this); }
};

//  ■
// ■■■
class BlockB : public Block
{
public:
    BlockB() : Block(3, 3) {}

    virtual void genShape() override
    {
        _shape[0][1] = 1;
        _shape[1][0] = 1;
        _shape[1][1] = 1;
        _shape[2][1] = 1;
    }

    virtual void turn() override
    {
        if (_status == 0)
        {
            rotate();
            ShiftHorizontal(1);
            _status = 1;
        }
        else if (_status == 1)
        {
            rotate();
            ShiftVertical(1);
            _status = 2;
        }
        else if (_status == 2)
        {
            rotate();
            _status = 3;
        }
        else if (_status == 3)
        {
            rotate();
            _status = 0;
        }
    }

    virtual BlockB* clone() override { return new BlockB(*this); }
};

Block* BlockFactory(int type)
{
    if (type == 0)
    {
        Block* b = new BlockA();
        b->genShape();
        b->genColor();
        return b;
    }
    else if (type == 1)
    {
        Block* b = new BlockB();
        b->genShape();
        b->genColor();
        return b;
    }

    return nullptr;
}

class Clock
{
public:
    Clock(float fps = 2.0) :_fps(fps) {}

    void setFps(float fps) { _fps = fps; }
    float getFps() { return _fps; }

    bool tick()
    {
        std::chrono::steady_clock::time_point cur = std::chrono::steady_clock::now();

        float during =
            std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1, 1000>>>(cur - last).count();

        if (during > (1000 / _fps))
        {
            last = std::chrono::steady_clock::now();
            return true;
        }

        return false;
    }

private:
    std::chrono::steady_clock::time_point last;
    float _fps; //一秒下落格子数
};

class TetrisGame
{
public:
    int _gameW = 10, _gameH = 20; // game pane size
    int _offsetX, _offsetY; // game left top location

    const int borderSize = 1;

    std::shared_ptr<Block> _curBlock;
    std::shared_ptr<Block> _nextBlock;

    Wall _wall;
    Pane _pane;

    int _curTop;

    void eliminate()
    {
        if (_curBlock->_locY + _curBlock->validRow() < _curTop)
            _curTop = _curBlock->_locY + _curBlock->validRow();

        int cur = _curTop;
        int bottom = min(_curBlock->_locY + _curBlock->_shape.size(), _pane._shape.size() - 1);

        bool needRepaint = false;

        std::shared_ptr<Block> tmp(_pane.clone());

        for (int i = cur; i <= bottom; i++)
        {
            int cnt = std::count(_pane._shape[i].begin(), _pane._shape[i].end(), 0);
            if (cnt == 0) //找不到空缺，说明填满了这一行
            {
                for (int j = i; j > _curTop; j--) //依次下落
                {
                    _pane._shape[j] = _pane._shape[j - 1];
                }

                for (int j = 0; j < _pane._shape[_curTop].size(); j++) //清空最上一行
                {
                    _pane._shape[_curTop][j] = 0;
                }

                needRepaint = true;

                _curTop++;
            }
        }

        if (needRepaint)
            tmp->repaint();
    }

    bool init()
    {
        _wall.setSize(_gameW + 2, _gameH + 2);
        _wall.setParent(nullptr);
        _wall.setLoc(_offsetX, _offsetY);
        _wall.genShape();

        _pane.setSize(10, 20);
        _pane.setParent(&_wall);
        _pane.setLoc(borderSize, borderSize);
        _pane.genShape();

        return 0;
    }

    TetrisGame()
        :_clock()
    {
        _clock.setFps(4);
        Console::setCursorVisible(false);
        _offsetX = 4, _offsetY = 4;
        srand(time(0));

        init();
    }

    Block* randomGenBlock()
    {
        Block* b = BlockFactory(rand() % 2);
        b->setParent(&_pane);
        b->setLoc((_pane._shape[0].size() - b->Shape()[0].size()) / 2, -1 * b->Shape().size() + b->validRow());

        return b;
    }

    std::map<int, bool (TetrisGame::*)() > _keyActionMap{
        {Console::KEY_NO, &TetrisGame::doNothing},
        {Console::KEY_UP, &TetrisGame::doRotate},
        {Console::KEY_DOWN, &TetrisGame::doDown},
        {Console::KEY_LEFT, &TetrisGame::doLeft},
        {Console::KEY_RIGHT, &TetrisGame::doRight},
    };

    bool doNothing()
    {
        return true;
    }

    // return false present merge， the block is disappear.
    bool doRotate()
    {
        std::shared_ptr<Block> tmp(_curBlock->clone());
        tmp->rotate();

        if (!_pane.isconflict(*tmp) && !_wall.isconflict(*tmp))
        {
            _curBlock->repaint();
            _curBlock->rotate();
        }

        return true;
    }

    bool doLeft()
    {
        std::shared_ptr<Block> tmp(_curBlock->clone());
        tmp->_locX--;

        if (!_pane.isconflict(*tmp) && !_wall.isconflict(*tmp))
        {
            _curBlock->repaint();
            _curBlock->_locX--;
        }

        return true;
    }

    bool doRight()
    {
        std::shared_ptr<Block> tmp(_curBlock->clone());
        tmp->_locX++;

        if (!_pane.isconflict(*tmp) && !_wall.isconflict(*tmp))
        {
            _curBlock->repaint();
            _curBlock->_locX++;
        }

        return true;
    }

    bool doDown()
    {
        std::shared_ptr<Block> tmp(_curBlock->clone());
        tmp->_locY++;
        if (_pane.isconflict(*tmp) || _wall.isconflict(*tmp))
        {
            _pane.merge(*_curBlock);
            eliminate();
            return false;
        }
        else {
            _curBlock->repaint();
            _curBlock->_locY++;
        }
        return true;
    }

    bool gameover()
    {
        std::shared_ptr<Block> tmp(_curBlock->clone());
        tmp->_locY++;
        return _pane.isconflict(*tmp) && (_curBlock->_locY + _curBlock->validRow()) < 0;
    }

    void run()
    {
        _wall.paint();
        _pane.paint();

        _curTop = _gameH;

        this->_curBlock.reset(randomGenBlock());

        queue<Console::Key> actions;

        bool playing = true;

        while (playing)
        {
            //Console::setCursorVisible(false);
            if (_clock.tick())
                actions.push(Console::KEY_DOWN);

            actions.push(Console::getKey());

            while (!actions.empty())
            {
                Console::Key k = actions.front();
                actions.pop();

                if (k == Console::KEY_DOWN && gameover())
                {
                    cout << "Game over" << endl;
                    playing = false;
                    break;
                }

                if (_keyActionMap.find(k) != _keyActionMap.end())
                {
                    if ( !(this->*_keyActionMap[k])() )
                    {
                        this->_curBlock.reset(randomGenBlock());
                        while (!actions.empty()) // new block, clear all actions which effect on current block
                            actions.pop();
                    }
                }
            }

            _wall.paint();
            _pane.paint();
            _curBlock->paint();
        }
    }

private:
    Clock _clock;
};

int main()
{
    TetrisGame tg;
    tg.run();
    getchar();
    return 0;
}
