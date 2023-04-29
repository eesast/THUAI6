# Algorithm

---

天梯分数计算算法

原始记录在：<https://github.com/eesast/THUAI5/discussions/86>

内容如下：

## THUAI4

关于根据队式每场比赛的分数映射到天梯分数的问题：  
队式比赛为两队对战，每队得分的区间均为 [0, 2500]。
以 tanh 函数为基础进行设计。  
设计原则如下：  

1. 输的扣少量天梯分，赢的得大量天梯分
2. 本就有极高天梯分数的虐本就天梯分数低的，这种降维打击现象，天梯分数涨幅极小甚至不涨天梯分
3. 如果在某场比赛中，两者表现差不多，即赢的比输的得分高得不多的话，那么天梯分数涨幅也不是很高
4. 如果本来天梯分数很低的，赢了天梯分数很高的，那么他得到的天梯分会较高，而另一个人，天梯分数降分稍多一些
5. 如果天梯分数低的赢了天梯分数高的，但是这场比赛赢得不多的话，会把两人的分数向中间靠拢
6. 总体上，赢的队伍不会降天梯分；输的队伍不会加天梯分
7. 其他条件相同的情况下，在本场游戏中得分越多，加的天梯分数也越高

上述原则可以保证以下两个目的的达成：

1. 总体来看，进行的游戏场次越多，所有队伍的平均天梯分数就越高
2. 经过足够多次的游戏场次，实力有一定差距的队伍的天体分数差距逐渐拉开，实力相近的队伍的天梯分数不会差别过大，各支队伍的排名趋近于收敛

用 cpp 代码编写算法代码如下（`cal` 函数）：  

```cpp
#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

template <typename T>
using mypair = pair<T, T>;

// orgScore 是天梯中两队的分数；competitionScore 是这次游戏两队的得分

mypair<int> cal(mypair<int> orgScore, mypair<int> competitionScore)
{

    // 调整顺序，让第一个元素成为获胜者，便于计算

    bool reverse = false;    // 记录是否需要调整

    if (competitionScore.first < competitionScore.second)
    {
        reverse = true;
    }
    else if (competitionScore.first == competitionScore.second)
    {
        if (orgScore.first == orgScore.second)      // 完全平局，不改变天梯分数
        {
            return orgScore;
        }

        if (orgScore.first > orgScore.second)        // 本次游戏平局，但一方天梯分数高，另一方天梯分数低，需要将两者向中间略微靠拢，因此天梯分数低的定为获胜者
        {
            reverse = true;
        }
        else
        {
            reverse = false;
        }
    }
        
    if (reverse)   // 如果需要换，换两者的顺序
    {
        swap(competitionScore.first, competitionScore.second);
        swap(orgScore.first, orgScore.second);
    }


    // 转成浮点数
    mypair<double> orgScoreLf;
    mypair<double> competitionScoreLf;
    orgScoreLf.first = orgScore.first;
    orgScoreLf.second = orgScore.second;
    competitionScoreLf.first = competitionScore.first;
    competitionScoreLf.second = competitionScore.second;
    mypair<int> resScore;

    const double deltaWeight = 80.0;       // 差距悬殊判断参数，比赛分差超过此值就可以认定为非常悬殊了，天梯分数增量很小，防止大佬虐菜鸡的现象造成两极分化

    double delta = (orgScoreLf.first - orgScoreLf.second) / deltaWeight;
    cout << "Tanh delta: " << tanh(delta) << endl;
    {

        const double firstnerGet = 8e-5;           // 胜利者天梯得分权值
        const double secondrGet = 5e-6;            // 失败者天梯得分权值

        double deltaScore = 100.0;        // 两队竞争分差超过多少时就认为非常大
        double correctRate = (orgScoreLf.first - orgScoreLf.second) / 100.0;       // 订正的幅度，该值越小，则在势均力敌时天梯分数改变越大
        double correct = 0.5 * (tanh((competitionScoreLf.first - competitionScoreLf.second - deltaScore) / deltaScore - correctRate) + 1.0);      // 一场比赛中，在双方势均力敌时，减小天梯分数的改变量

        resScore.first = orgScore.first + round(competitionScoreLf.first * competitionScoreLf.first * firstnerGet * (1 - tanh(delta)) * correct);  // 胜者所加天梯分
        resScore.second = orgScore.second - round((2500.0 - competitionScoreLf.second) * (2500.0 - competitionScoreLf.second) * secondrGet * (1 - tanh(delta)) * correct);  // 败者所扣天梯分，2500 为得分的最大值（THUAI4 每场得分介于 0~2500 之间）
    }

    // 如果换过，再换回来
    if (reverse)
    {
        swap(resScore.first, resScore.second);
    }

    return resScore;
}
```

**特别注意**：此算法是在 THUAI4 的比赛直接得分封顶为 2500 分、最低不低于 0 分的前提下设计的，因此并不一定适用于 THUAI5 的情形。  

## THUAI5

今年把得分上限这个东西去掉了。理论上今年可以得很高很高分，但是我估计大部分比赛得分在400-600左右，最高估计1000左右。算法 借 鉴 了THUAI4,算法，换了个激活函数（正态CDF），感觉分数变化相对更好了一些？
代码如下：

```cpp
#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

template <typename T>
using mypair = pair<T, T>;

double PHI(double x) // THUAI3: Sigmoid; THUAI4: Tanh; THUAI5: Normal Distribution CDF
{
    //double a1 = 0.2548292592;
    //double a2 = -0.284496736;
    //double a3 = 1.421413741;
    //double a4 = -1.453152027;
    //double a5 = 1.061405429;
    //double p = 0.3275911;
    //int sign = 1;
    //if (x < 0)
    //    sign = -1;
    //x = fabs(x) / sqrt(2.0);
    //double t = 1.0 / (1.0 + p * x);
    //double y = 1.0 - ((((((a5 * t + a4) * t + a3) * t) + a2) * t) + a1) * t * exp(-x * x);
    //double cdf = 0.5 * (1.0 + sign * y);
    //return (cdf - 0.5) * 2.0; // 化到[-1,1]之间

    return erf(x / sqrt(2));
}

// orgScore 是天梯中两队的分数；competitionScore 是这次游戏两队的得分
mypair<int> cal(mypair<int> orgScore, mypair<int> competitionScore)
{

    // 调整顺序，让第一个元素成为获胜者，便于计算

    bool reverse = false;    // 记录是否需要调整

    if (competitionScore.first < competitionScore.second)
    {
        reverse = true;
    }
    else if (competitionScore.first == competitionScore.second)
    {
        if (orgScore.first == orgScore.second)      // 完全平局，不改变天梯分数
        {
            return orgScore;
        }

        if (orgScore.first > orgScore.second)        // 本次游戏平局，但一方天梯分数高，另一方天梯分数低，需要将两者向中间略微靠拢，因此天梯分数低的定为获胜者
        {
            reverse = true;
        }
        else
        {
            reverse = false;
        }
    }

    if (reverse)   // 如果需要换，换两者的顺序
    {
        swap(competitionScore.first, competitionScore.second);
        swap(orgScore.first, orgScore.second);
    }


    // 转成浮点数
    mypair<double> orgScoreLf;
    mypair<double> competitionScoreLf;
    orgScoreLf.first = orgScore.first;
    orgScoreLf.second = orgScore.second;
    competitionScoreLf.first = competitionScore.first;
    competitionScoreLf.second = competitionScore.second;
    mypair<int> resScore;

    const double deltaWeight = 90.0;       // 差距悬殊判断参数，比赛分差超过此值就可以认定为非常悬殊了，天梯分数增量很小，防止大佬虐菜鸡的现象造成两极分化

    double delta = (orgScoreLf.first - orgScoreLf.second) / deltaWeight;
    cout << "Normal CDF delta: " << PHI(delta) << endl;
    {

        const double firstnerGet = 3e-4;           // 胜利者天梯得分权值
        const double secondrGet = 1e-4;            // 失败者天梯得分权值

        double deltaScore = 100.0;        // 两队竞争分差超过多少时就认为非常大
        double correctRate = (orgScoreLf.first - orgScoreLf.second) / 100.0;       // 订正的幅度，该值越小，则在势均力敌时天梯分数改变越大
        double correct = 0.5 * (PHI((competitionScoreLf.first - competitionScoreLf.second - deltaScore) / deltaScore - correctRate) + 1.0);      // 一场比赛中，在双方势均力敌时，减小天梯分数的改变量

        resScore.first = orgScore.first + round(competitionScoreLf.first * competitionScoreLf.first * firstnerGet * (1 - PHI(delta)) * correct);  // 胜者所加天梯分
        if (competitionScoreLf.second < 1000)
            resScore.second = orgScore.second - round((1000.0 - competitionScoreLf.second) * (1000.0 - competitionScoreLf.second) * secondrGet * (1 - PHI(delta)) * correct);  // 败者所扣天梯分
        else
            resScore.second = orgScore.second; // 败者拿1000分，已经很强了，不扣分
    }

    // 如果换过，再换回来
    if (reverse)
    {
        swap(resScore.first, resScore.second);
    }

    return resScore;
}

void Print(mypair<int> score)
{
    std::cout << " team1: " << score.first << std::endl
        << "team2: " << score.second << std::endl;
}

int main()
{
    int x, y;
    std::cout << "origin score of team 1 and 2: " << std::endl;
    std::cin >> x >> y;
    auto ori = mypair<int>(x, y);
    std::cout << "game score of team 1 and 2: " << std::endl;
    std::cin >> x >> y;
    auto sco = mypair<int>(x, y);
    Print(cal(ori, sco));
}
```

`1000 - score`（x  
`ReLU(1000 - score)`（√  
防止真的超过了 1000）

## THUAI6

### high-ladder

因为今年的对局得分是两局得分之和，所以会出现一定程度的“数值膨胀”，在这里调低了胜者得分权值，同时提高了比赛分差距悬殊阈值和天梯分差距悬殊阈值。同时由于今年得分的上限不好确定，所以负者失分的基础值变为与胜者的得分之差。

```c++
#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

template <typename T>
using mypair = pair<T, T>;

// orgScore 是天梯中两队的分数；competitionScore 是这次游戏两队的得分

mypair<int> cal(mypair<int> orgScore, mypair<int> competitionScore)
{

    // 调整顺序，让第一个元素成为获胜者，便于计算

    bool reverse = false; // 记录是否需要调整

    if (competitionScore.first < competitionScore.second)
    {
        reverse = true;
    }
    else if (competitionScore.first == competitionScore.second)
    {
        if (orgScore.first == orgScore.second) // 完全平局，不改变天梯分数
        {
            return orgScore;
        }

        if (orgScore.first > orgScore.second) // 本次游戏平局，但一方天梯分数高，另一方天梯分数低，需要将两者向中间略微靠拢，因此天梯分数低的定为获胜者
        {
            reverse = true;
        }
        else
        {
            reverse = false;
        }
    }

    if (reverse) // 如果需要换，换两者的顺序
    {
        swap(competitionScore.first, competitionScore.second);
        swap(orgScore.first, orgScore.second);
    }

    // 转成浮点数
    mypair<double> orgScoreLf;
    mypair<double> competitionScoreLf;
    orgScoreLf.first = orgScore.first;
    orgScoreLf.second = orgScore.second;
    competitionScoreLf.first = competitionScore.first;
    competitionScoreLf.second = competitionScore.second;
    mypair<int> resScore;

    const double deltaWeight = 1000.0; // 差距悬殊判断参数，比赛分差超过此值就可以认定为非常悬殊了，天梯分数增量很小，防止大佬虐菜鸡的现象造成两极分化

    double delta = (orgScoreLf.first - orgScoreLf.second) / deltaWeight;
    cout << "Tanh delta: " << tanh(delta) << endl;
    {

        const double firstnerGet = 9e-6; // 胜利者天梯得分权值
        const double secondrGet = 5e-6;  // 失败者天梯得分权值

        double deltaScore = 2100.0;                                                                                                          // 两队竞争分差超过多少时就认为非常大
        double correctRate = (orgScoreLf.first - orgScoreLf.second) / (deltaWeight * 1.2);                                                   // 订正的幅度，该值越小，则在势均力敌时天梯分数改变越大
        double correct = 0.5 * (tanh((competitionScoreLf.first - competitionScoreLf.second - deltaScore) / deltaScore - correctRate) + 1.0); // 一场比赛中，在双方势均力敌时，减小天梯分数的改变量
        cout << "correct: " << correct << endl;
        resScore.first = orgScore.first + round(competitionScoreLf.first * competitionScoreLf.first * firstnerGet * (1 - tanh(delta)) * correct);                                                              // 胜者所加天梯分
        resScore.second = orgScore.second - round((competitionScoreLf.first - competitionScoreLf.second) * (competitionScoreLf.first - competitionScoreLf.second) * secondrGet * (1 - tanh(delta)) * correct); // 败者所扣天梯分
    }

    // 如果换过，再换回来
    if (reverse)
    {
        swap(resScore.first, resScore.second);
    }

    return resScore;
}
```

### competition

与天梯得分算法要满足的“枫氏七条”类似，比赛得分算法也要满足“唐氏四律”，分别如下：

1. 两队经过某场比赛的得分变化，应只与该场比赛有关，而与历史积分无关。
2. 须赋予比赛获胜一方基础得分，哪怕获胜一方的优势非常小。也就是说，哪怕胜利一方仅以微弱优势获胜，也需要拉开胜者与败者的分差。
3. 胜利一方优势越大，得分理应越高。
4. 对于一场比赛，胜利一方的得分不能无限大，须控制在一个合理的数值以下。

- 在非平局的情况下，（胜者）天梯得分与双方比赛分差值成正相关，得分函数如下（以x表示得分差值，y表示（胜者）天梯得分，a、b为固定参数）

​											$$y=ax^2(1-0.375\cdot(\tanh(\frac{x}{b}-1)+1))$$

- 在平局情况下，（双方）天梯得分与比赛分成正相关，得分函数如下（以x表示比赛分，y表示（双方）天梯得分，c为固定参数）

​											$$y=cx^2$$

- 不管是哪种情况，都有得分下界，非平局为100，平局为25

```c++
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cassert>
using namespace std;

template <typename T>
using mypair = pair<T, T>;
double minScore = 100;

double TieScore(double gameScore)
{
    const double get = 9e-5;                                                          // 天梯得分权值
    double deltaScore = 2000.0;                                                       // 订正的幅度，该值越小，则在双方分差较大时，天梯分数改变量越小，整个函数的变化范围大约为0~2*deltaScore
    double highScore = 6000.0;                                                        // 将highScore设定为较大值，使得correct较小
    double correct = 1 - 0.375 * (tanh((highScore - deltaScore) / deltaScore) + 1.0); // 一场比赛中，在双方分差较大时，减小天梯分数的改变量
    cout << "correct: " << correct << endl;
    int score = round(gameScore * gameScore * get * correct / 4);
    return score > minScore / 4 ? score : minScore / 4;
}

double WinScore(double delta, double winnerGameScore) // 根据游戏得分差值，与绝对分数，决定最后的加分
{
    assert(delta > 0);
    const double firstnerGet = 9e-5;                                              // 胜利者天梯得分权值
    double deltaScore = 2000.0;                                                   // 订正的幅度，该值越小，则在双方分差较大时，天梯分数改变量越小，整个函数的变化范围大约为0~2*deltaScore
    double correct = 1 - 0.375 * (tanh((delta - deltaScore) / deltaScore) + 1.0); // 一场比赛中，在双方分差较大时，减小天梯分数的改变量
    cout << "correct: " << correct << endl;
    int score = round(delta * delta * firstnerGet * correct);
    return score > minScore ? score : minScore;
}

// orgScore 是天梯中两队的分数；competitionScore 是这次游戏两队的得分
mypair<double> cal(mypair<double> orgScore, mypair<double> competitionScore)
{
    // 调整顺序，让第一个元素成为获胜者，便于计算

    bool reverse = false; // 记录是否需要调整

    if (competitionScore.first < competitionScore.second)
    {
        reverse = true;
    }

    if (reverse) // 如果需要换，换两者的顺序
    {
        swap(competitionScore.first, competitionScore.second);
        swap(orgScore.first, orgScore.second);
    }

    double delta = competitionScore.first - competitionScore.second;
    double addScore;
    mypair<double> resScore;

    // 先处理平局的情况
    if (delta == 0)
    {
        addScore = TieScore(competitionScore.first);
        resScore = mypair<double>(orgScore.first + addScore, orgScore.second + addScore);
    }

    // 再处理有胜负的情况
    else
    {
        addScore = WinScore(delta, competitionScore.first);
        resScore = mypair<double>(orgScore.first + addScore, orgScore.second);
    }

    // 如果换过，再换回来
    if (reverse)
    {
        swap(resScore.first, resScore.second);
    }

    return resScore;
}

void Print(mypair<double> score)
{
    std::cout << "team1: " << score.first << std::endl
              << "team2: " << score.second << std::endl;
}

int main()
{
    double x, y, t, i = 0;
    cin >> t;
    while (i < t)
    {
        cout << "----------------------------------------\n";
        std::cout << "origin score of team 1 and 2: " << std::endl;
        std::cin >> x >> y;
        auto ori = mypair<double>(x, y);
        std::cout << "game score of team 1 and 2: " << std::endl;
        std::cin >> x >> y;
        auto sco = mypair<double>(x, y);
        Print(cal(ori, sco));
        ++i;
    }
    return 0;
}
```

