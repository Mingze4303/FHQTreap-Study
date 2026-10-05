#include <iostream>
#include <random>
using namespace std;

// ==================== FHQ Treap 的节点与全局状态 ====================
const int N = 1e5 + 10; // 节点数组容量上限（比操作数多留一些空间）
int ch[N][2], val[N], sz[N]; // ch[u][0/1]：左/右儿子编号；val[u]：节点键值；sz[u]：以 u 为根的子树节点数
unsigned int pri[N]; // pri[u]：节点 u 的随机堆优先级，用于维持 Treap 的平衡
int root, tot; // root：当前 Treap 根节点编号；tot：已分配的节点总数，也是新节点编号计数器
mt19937 rng(114514); // 随机数生成器；固定种子使程序结果可复现

// 创建一个键值为 x 的新节点，并返回其节点编号。
int newnode(int x) // x：新节点保存的键值
{
    tot++; // 分配一个从未使用过的节点编号
    val[tot] = x; // 保存该节点的键值
    sz[tot] = 1; // 新节点尚无儿子，子树大小为 1
    pri[tot] = rng(); // 随机生成优先级，避免树形退化
    return tot; // 返回新节点编号
}

// 根据左右子树的大小，重新计算节点 u 的子树大小。
void pushup(int u) // u：需要更新子树大小的节点编号
{
    sz[u] = sz[ch[u][0]] + sz[ch[u][1]] + 1; // 左子树大小 + 右子树大小 + 当前节点
    return;
}

// 按键值 x 将以 u 为根的树拆成两棵：a 中所有键值 <= x，b 中所有键值 > x。
void split(int u, int x, int& a, int& b) // u：待拆分树根；x：键值边界；a、b：通过引用返回的两棵树的根编号
{
    if (u == 0) // 空树拆分后仍是两棵空树
    {
        a = 0;
        b = 0;
        return;
    }
    if (val[u] <= x) // 当前节点属于左侧结果 a；只需继续拆它的右子树
    {
        a = u;
        split(ch[u][1], x, ch[u][1], b); // 拆分右子树，较小部分接回当前节点，较大部分作为 b
    }
    else // 当前节点属于右侧结果 b；只需继续拆它的左子树
    {
        b = u;
        split(ch[u][0], x, a, ch[u][0]); // 拆分左子树，较小部分作为 a，较大部分接回当前节点
    }
    pushup(u); // 子树结构改变后，更新当前节点的子树大小
    return;
}

// 合并两棵树，要求 a 中所有键值均 <= b 中所有键值；返回合并后的根编号。
int merge(int a, int b) // a、b：待合并的两棵树的根节点编号
{
    if (a == 0 || b == 0) // 任意一棵树为空时，合并结果就是另一棵树
    {
        return a + b;
    }
    if (pri[a] < pri[b]) // a 的优先级更高，按堆性质由 a 作为合并后的根
    {
        ch[a][1] = merge(ch[a][1], b); // b 中的键值不小于 a，递归合并到 a 的右子树
        pushup(a); // 更新 a 的子树节点数
        return a;
    }
    else // b 的优先级更高，由 b 作为合并后的根
    {
        ch[b][0] = merge(a, ch[b][0]); // a 中的键值不大于 b，递归合并到 b 的左子树
        pushup(b); // 更新 b 的子树节点数
        return b;
    }
}

// 查询以 u 为根的树中按键值升序排列的第 k 个元素（k 从 1 开始）。
int kth(int u, int k) // u：查询树的根节点；k：目标名次
{
    while (u) // 沿树查找，直到命中目标节点或走到空节点
    {
        if (k <= sz[ch[u][0]]) // 目标名次落在左子树中
        {
            u = ch[u][0];
        }
        else if (k == sz[ch[u][0]] + 1) // 左子树之后的第一个元素就是当前节点
        {
            return val[u];
        }
        else // 目标名次在右子树中，扣除左子树和当前节点的元素数
        {
            k -= sz[ch[u][0]] + 1;
            u = ch[u][1];
        }
    }
    return 0; // k 超出有效范围时的兜底返回值；题目保证查询合法
}

// 查询严格小于 x 的最大元素（前驱）。
int pre(int x) // x：查询边界
{
    int a, b; // a：键值 < x 的树；b：键值 >= x 的树
    split(root, x - 1, a, b); // 按 x-1 拆分，整数键值下 a 恰好包含所有小于 x 的元素
    int res = kth(a, sz[a]); // a 中最后一个元素即为前驱
    root = merge(a, b); // 查询拆分后需合并回去，恢复完整 Treap
    return res; // 返回前驱值
}

// 查询严格大于 x 的最小元素（后继）。
int nxt(int x) // x：查询边界
{
    int a, b; // a：键值 <= x 的树；b：键值 > x 的树
    split(root, x, a, b); // 按 x 拆分，b 中保存所有严格大于 x 的元素
    int res = kth(b, 1); // b 中第一个元素即为后继
    root = merge(a, b); // 查询拆分后需合并回去，恢复完整 Treap
    return res; // 返回后继值
}

// 逐条读取并执行 P3369 的六类平衡树操作。
int main()
{
    int n; // 操作总数
    cin >> n;
    for (int i = 1; i <= n; i++) // 按输入顺序处理 n 条操作
    {
        int op, x; // op：操作类型（1~6）；x：插入/删除的数值或查询参数
        cin >> op >> x;
        if (op == 1) // 插入一个数 x
        {
            int a, b; // a：所有 <= x 的节点；b：所有 > x 的节点
            split(root, x, a, b); // 为新节点找到按键值排序的插入位置
            root = merge(merge(a, newnode(x)), b); // 插入新节点并合并回整棵树
        }
        else if (op == 2) // 删除一个数 x（题目保证 x 存在）
        {
            int a, b, c; // a：键值 < x 的树；c：键值 == x 的节点树；b：键值 > x 的树
            split(root, x, a, b); // 先把 <= x 与 > x 分开
            split(a, x - 1, a, c); // 再把 < x 与 == x 分开；c 的根节点是一个待删除的 x
            c = merge(ch[c][0], ch[c][1]); // 删除 c 的根节点，并合并其左右子树；若有重复值则其余节点保留
            root = merge(merge(a, c), b); // 将小于 x、替代子树和大于 x 的部分依次合并
        }
        else if (op == 3) // 查询 x 的排名（排名从 1 开始）
        {
            int a, b; // a：键值 < x；b：键值 >= x
            split(root, x - 1, a, b); // 把所有小于 x 的元素分到 a
            cout << sz[a] + 1 << "\n"; // x 的排名 = 小于 x 的元素个数 + 1
            root = merge(a, b); // 恢复完整 Treap
        }
        else if (op == 4) // 查询排名为 x 的数
        {
            cout << kth(root, x) << "\n"; // 直接按子树大小查找第 x 小元素
        }
        else if (op == 5) // 查询 x 的前驱（严格小于 x 的最大值）
        {
            cout << pre(x) << "\n"; // pre 内部会拆分并恢复 Treap
        }
        else // op == 6：查询 x 的后继（严格大于 x 的最小值）
        {
            cout << nxt(x) << "\n"; // nxt 内部会拆分并恢复 Treap
        }
    }
    return 0; // 程序正常结束
}