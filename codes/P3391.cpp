#include <iostream>
#include <random>
using namespace std;

const int N = 1e5 + 10;
int ch[N][2], val[N], sz[N], rev[N];
unsigned int pri[N];
int rt, tot;
mt19937 rng(114514);

int newnode(int x)
{
    tot++;
    val[tot] = x;
    sz[tot] = 1;
    rev[tot] = 0;
    pri[tot] = rng();
    return tot;
}

void pushup(int u)
{
    sz[u] = sz[ch[u][0]] + sz[ch[u][1]] + 1;
    return;
}

void pushdown(int u)
{
    if (rev[u])
    {
        swap(ch[u][0], ch[u][1]);
        rev[ch[u][0]] ^= 1;
        rev[ch[u][1]] ^= 1;
        rev[u] = 0;
    }
    return;
}

void split(int u, int k, int& a, int& b)
{
    if (u == 0)
    {
        a = 0;
        b = 0;
        return;
    }
    pushdown(u);
    if (sz[ch[u][0]] >= k)
    {
        b = u;
        split(ch[u][0], k, a, ch[u][0]);
    }
    else
    {
        a = u;
        split(ch[u][1], k - sz[ch[u][0]] - 1, ch[u][1], b);
    }
    pushup(u);
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
        pushdown(a);
        ch[a][1] = merge(ch[a][1], b);
        pushup(a);
        return a;
    }
    else
    {
        pushdown(b);
        ch[b][0] = merge(a, ch[b][0]);
        pushup(b);
        return b;
    }
}

void print(int u)
{
    if (u == 0)
    {
        return;
    }
    pushdown(u);
    print(ch[u][0]);
    cout << val[u] << " ";
    print(ch[u][1]);
    return;
}

int main()
{
    int n, m;
    cin >> n >> m;
    for (int i = 1; i <= n; i++)
    {
        rt = merge(rt, newnode(i));
    }
    for (int i = 1; i <= m; i++)
    {
        int l, r;
        cin >> l >> r;
        int a, b, c;
        split(rt, l - 1, a, b);
        split(b, r - l + 1, b, c);
        rev[b] ^= 1;
        rt = merge(a, merge(b, c));
    }
    print(rt);
    cout << "\n";
    return 0;
}