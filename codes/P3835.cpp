#include <iostream>
#include <random>
using namespace std;

const int N = 5e5 + 10;
const int M = 5e7 + 10;
int ch[M][2], val[M], sz[M];
unsigned int pri[M];
int rt[N], tot;
mt19937 rng(114514);

int newnode(int x)
{
    tot++;
    ch[tot][0] = 0;
    ch[tot][1] = 0;
    val[tot] = x;
    sz[tot] = 1;
    pri[tot] = rng();
    return tot;
}

int clone(int u)
{
    tot++;
    ch[tot][0] = ch[u][0];
    ch[tot][1] = ch[u][1];
    val[tot] = val[u];
    sz[tot] = sz[u];
    pri[tot] = pri[u];
    return tot;
}

void pushup(int u)
{
    sz[u] = sz[ch[u][0]] + sz[ch[u][1]] + 1;
    return;
}

void split(int u, int x, int& a, int& b)
{
    if (u == 0)
    {
        a = 0;
        b = 0;
        return;
    }
    if (val[u] <= x)
    {
        a = clone(u);
        split(ch[a][1], x, ch[a][1], b);
        pushup(a);
    }
    else
    {
        b = clone(u);
        split(ch[b][0], x, a, ch[b][0]);
        pushup(b);
    }
    return;
}

int merge(int a, int b)
{
    if (a == 0 || b == 0)
    {
        return a + b;
    }
    if (pri[a] < pri[b])
    {
        int c = clone(a);
        ch[c][1] = merge(ch[a][1], b);
        pushup(c);
        return c;
    }
    else
    {
        int c = clone(b);
        ch[c][0] = merge(a, ch[b][0]);
        pushup(c);
        return c;
    }
}

int kth(int u, int k)
{
    while (u)
    {
        if (k <= sz[ch[u][0]])
        {
            u = ch[u][0];
        }
        else if (k == sz[ch[u][0]] + 1)
        {
            return val[u];
        }
        else
        {
            k -= sz[ch[u][0]] + 1;
            u = ch[u][1];
        }
    }
    return 0;
}

int rnk(int u, int x)
{
    int res = 1;
    while (u)
    {
        if (val[u] < x)
        {
            res += sz[ch[u][0]] + 1;
            u = ch[u][1];
        }
        else
        {
            u = ch[u][0];
        }
    }
    return res;
}

int pre(int u, int x)
{
    int res = -2147483647;
    while (u)
    {
        if (val[u] < x)
        {
            res = val[u];
            u = ch[u][1];
        }
        else
        {
            u = ch[u][0];
        }
    }
    return res;
}

int nxt(int u, int x)
{
    int res = 2147483647;
    while (u)
    {
        if (val[u] > x)
        {
            res = val[u];
            u = ch[u][0];
        }
        else
        {
            u = ch[u][1];
        }
    }
    return res;
}

int main()
{
    int n;
    cin >> n;
    for (int i = 1; i <= n; i++)
    {
        int v, op, x;
        cin >> v >> op >> x;
        if (op == 1)
        {
            int a, b;
            split(rt[v], x, a, b);
            rt[i] = merge(merge(a, newnode(x)), b);
        }
        else if (op == 2)
        {
            if (rnk(rt[v], x) == rnk(rt[v], x + 1))
            {
                rt[i] = rt[v];
            }
            else
            {
                int a, b, c;
                split(rt[v], x, a, b);
                split(a, x - 1, a, c);
                c = merge(ch[c][0], ch[c][1]);
                rt[i] = merge(merge(a, c), b);
            }
        }
        else if (op == 3)
        {
            cout << rnk(rt[v], x) << "\n";
            rt[i] = rt[v];
        }
        else if (op == 4)
        {
            if (sz[rt[v]] < x)
            {
                cout << 2147483647 << "\n";
            }
            else
            {
                cout << kth(rt[v], x) << "\n";
            }
            rt[i] = rt[v];
        }
        else if (op == 5)
        {
            cout << pre(rt[v], x) << "\n";
            rt[i] = rt[v];
        }
        else
        {
            cout << nxt(rt[v], x) << "\n";
            rt[i] = rt[v];
        }
    }
    return 0;
}