#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <set>
#include <vector>

struct edge
{
    int from;
    int to;
    long long dist;

    edge(int from, int to, long long dist)
        : from(from)
        , to(to)
        , dist(dist)
    {}

    edge() = default;
};

bool operator<(edge const& a, edge const& b)
{
    if (a.dist != b.dist)
        return a.dist < b.dist;
    else if (a.from != b.from)
        return a.from < b.from;
    else
        return a.to < b.to;
}


struct Node
{
    explicit Node(edge data)
        : data_(data)
        , add(0)
    {
        left = right = nullptr;
    }

    edge data()
    {
        push();
        return data_;
    }

    void add_all(long long value)
    {
        add += value;
    }

private:
    edge data_;
    Node* left;
    Node* right;
    long long add;

    friend Node* merge(Node*, Node*);
    friend edge extract_min(Node*&);

    void push()
    {
        data_.dist += add;
        if (left)
            left->add += add;
        if (right)
            right->add += add;
        add = 0;
    }

};

Node* merge(Node* a, Node* b)
{
    if (a == nullptr)
        return b;
    if (b == nullptr)
        return a;

    a->push();
    b->push();

    if (b->data_ < a->data_)
    {
        std::swap(a, b);
    }
    // a >= b
    a->left = merge(a->left, b);
    std::swap(a->left, a->right);
    return a;
}

edge extract_min(Node*& t)
{
    edge ans = t->data();
    t = merge(t->left, t->right);
    return ans;
}

struct dsu
{
    explicit dsu(std::vector<Node*> data)
        : p(data.size())
        , size(data.size(), 1)
        , data(std::move(data))
    {
        for (int i = 0; i < this->data.size(); ++i)
        {
            p[i] = i;
        }
    }

    Node*& component(int index)
    {
        return data[get(index)];
    }

    int component_size(int index) const
    {
        return size[get(index)];
    }

    int get(int v) const
    {
        return p[v] == v ? v : p[v] = get(p[v]);
    }

    bool unite(int a, int b)
    {
        a = get(a);
        b = get(b);
        if (size[a] < size[b])
            std::swap(a, b);
        if (a == b)
            return false;
        p[b] = a;
        size[a] += size[b];
        data[a] = merge(data[a], data[b]);
        return true;
    }

private:
    mutable std::vector<int> p;
    std::vector<size_t> size;
    std::vector<Node*> data;

};


inline void solve(std::istream& is, std::ostream& os)
{
    int n, m;
    is >> n >> m;
    std::vector<Node*> data(n, nullptr);
    std::vector<std::list<int>> g(n);
    for (int i = 0; i < m; ++i)
    {
        int from, to;
        long long cost;
        is >> from >> to >> cost;
        from--, to--;
        data[to] = merge(new Node(edge(from, to, cost)), data[to]);
        g[from].push_back(to);
    }

    std::vector<bool> visited(n, false);

    std::function<void(int)> search = [&visited, &g, &search](int v)
    {
        visited[v] = true;
        for (int u : g[v])
        {
            if (!visited[u])
                search(u);
        }
    };

    search(0);
    if (std::find(visited.begin(), visited.end(), false) != visited.end())
    {
        os << "NO" << std::endl;
        return;
    }
    g.clear();

    long long ans = 0;

    dsu note(data);
    std::list<edge> st;

    std::function<void(int)> up = [&note, &visited, &ans, &up, &st](int x)
    {
        visited[note.get(x)] = true;
        edge e;
        do
        {
            e = extract_min(note.component(x));
        }
        while (note.get(e.from) == note.get(e.to));

        if (note.component(x) != nullptr)
            note.component(x)->add_all(-e.dist);
        st.push_back(e);

        if (!visited[e.from])
        {
            up(e.from);
        }
        else if (note.get(e.from) == note.get(0))
        {
            while (!st.empty())
            {
                ans += st.back().dist;
                note.unite(st.back().to, 0);
                st.pop_back();
            }
        }
        else
        {
            while (true)
            {
                auto t = st.back();
                st.pop_back();
                ans += t.dist;
                note.unite(t.from, e.from);
                if (note.get(t.to) == note.get(e.from))
                    break;
            }
            up(x);
        }
    };


    std::fill(visited.begin(), visited.end(), false);
    visited[0] = true;
    for (int i = 0; i < n; ++i)
    {
        if (note.get(i) != note.get(0))
        {
            up(i);
        }
    }

    os << "YES" << std::endl << ans << std::endl;
}

signed main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    solve(std::cin, std::cout);
    return 0;
}

