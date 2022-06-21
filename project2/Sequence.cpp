#include "Sequence.h"
#include <iostream>

Sequence::Node::Node(ItemType val, Node* prev, Node* next)
{
    this->val = val;
    this->prev = prev;
    this->next = next;
}

void Sequence::fill_sequence(const Sequence& other)
{
    num_items = other.num_items;
    head = nullptr;
    if(num_items > 0)
    {
        Node* other_iter = other.head;
        head = new Node(other.head->val);
        Node* iter = head;
        while(other_iter->next != nullptr)
        {
            iter->next = new Node(other_iter->next->val, iter);
            iter = iter->next;
            other_iter = other_iter->next;
        }
    }
}

void Sequence::reset_sequence()
{
    if(num_items > 0)
    {
        Node* iter = head;
        while(iter != nullptr)
        {
            Node* tmp = iter->next;
            delete iter;
            iter = tmp;
        }
    }
}

Sequence::Sequence() 
{
    num_items = 0;
    head = nullptr;
}

Sequence::Sequence(const Sequence& other)
{
    this->fill_sequence(other);
}

Sequence::~Sequence()
{
    this->reset_sequence();
}

Sequence& Sequence::operator=(const Sequence& other)
{
    if(this == &other)
        return *this;
    this->reset_sequence();
    this->fill_sequence(other);
    return *this;
}

bool Sequence::empty() const
{
    return num_items == 0;
}

int Sequence::size() const
{
    return num_items;
}

int Sequence::insert(int pos, const ItemType& value)
{
    if(pos < 0 || pos > num_items)
        return -1;
    // Special case for replacing the head node
    if(pos == 0)
    {
        Node* tmp = head;
        head = new Node(value,nullptr,tmp);
        if(tmp != nullptr)
            tmp->prev = head;
        num_items++;
        return 0;
    }
    // Find preceding spot (we will insert into the next spot)
    Node* iter = head;
    for(int i = 0; i < pos - 1; i++)
    {
        iter = iter->next;
    }
    // Insert new node into the next position
    Node* tmp = iter->next;
    iter->next = new Node(value, iter, tmp);
    if(tmp != nullptr)
        tmp->prev = iter->next;
    num_items++;
    return pos;
}

int Sequence::insert(const ItemType& value)
{
    // Determine where to insert
    int p = 0;
    Node* iter = head;
    for( ; p < num_items; p++)
    {
        if(value <= iter->val)
            break;
        iter = iter->next;
    }
    // Insert new item
    return this->insert(p, value);
}

Sequence::Node* Sequence::delete_head()
{
    Node* tmp = head->next;
    if(tmp != nullptr)
        tmp->prev = nullptr;
    delete head;
    head = tmp;
    num_items--;
    return tmp;    
}

Sequence::Node* Sequence::delete_node(Node* node)
{
    Node* tmp = node->next;
    if(node->prev != nullptr)
        node->prev->next = node->next;
    if(node->next != nullptr)
        node->next->prev = node->prev;
    delete node;
    num_items--;
    return tmp;
}

bool Sequence::erase(int pos)
{
    if(pos < 0 || pos >= num_items)
        return false;
    // Erase item
    if(pos == 0)
    {
        this->delete_head();
        return true;
    }
    else
    {
        Node* iter = head;
        for(int i = 0; i < pos; i++)
        {
            iter = iter->next;
        }
        this->delete_node(iter);
        return true;
    }
}

int Sequence::remove(const ItemType& value)
{
    int ctr = 0, i = 0;
    Node* iter = head;
    while(i < num_items)
    {
        if( iter->val == value)
        {
            ctr++;
            if(i == 0)
            {
                iter = this->delete_head();
            }
            else
            {   
                iter = this->delete_node(iter);
            }
        }
        else
        {
            i++;
            iter = iter->next;
        }
    }
    return ctr;
}

bool Sequence::get(int pos, ItemType& value) const
{
    if(pos < 0 || pos >= num_items)
        return false;
    Node* iter = head;
    for(int i = 0; i < pos; i++)
    {
        iter = iter->next;
    }
    value = iter->val;
    return true;
}

bool Sequence::set(int pos, const ItemType& value)
{
    if(pos < 0 || pos >= num_items)
        return false;
    Node* iter = head;
    for(int i = 0; i < pos; i++)
    {
        iter = iter->next;
    }
    iter->val = value;
    return true;
}

int Sequence::find(const ItemType& value) const
{
    int p = -1;
    Node* iter = head;
    for(int i = 0; i < num_items; i++)
    {
        if(iter->val == value)
        {
            p = i;
            break;
        }
        iter = iter->next;
    }
    return p;
}

void Sequence::swap(Sequence& other)
{
    Node* head_tmp = this->head;
    int num_items_tmp = this->num_items;

    head = other.head;
    num_items = other.num_items;

    other.head = head_tmp;
    other.num_items = num_items_tmp;
}

void Sequence::dump(bool multidump)
{
    Node* iter = head;
    std::cerr << "DUMPING: \n";
    while(iter != nullptr)
    {
        std::cerr << iter->val << " ";
        if(iter->next == nullptr) break;
        iter = iter->next;
    }
    std::cerr << std::endl;
    while(iter != nullptr && multidump)
    {
        std::cerr << iter->val << " ";
        iter = iter->prev;
    }
    if(multidump)
        std::cerr << std::endl;
}

// Gross O(n^2) implementation
// Determine if seq2 is a subsequence of seq1
int subsequence(const Sequence& seq1, const Sequence& seq2)
{
    int size1 = seq1.size(), size2 = seq2.size();
    if(size1 == 0 || size2 == 0)
        return -1;
    int iter = 0;
    ItemType val1, val2;
    while(iter <= size1 - size2)
    {
        seq1.get(iter, val1);
        seq2.get(0, val2);
        if (val1 == val2)
        {
            int i = iter + 1, j = 1;
            while(j < size2)
            {
                seq1.get(i, val1);
                seq2.get(j, val2);
                if(val1 != val2)
                    break;
                j++;
                i++;
            }
            if(j == size2)
                return iter;
        }
        iter++;
    }
    return -1;
}

void interleave(const Sequence& seq1, const Sequence& seq2, Sequence& result)
{
    Sequence tmp;
    int i = 0, j = 0, k = 0;
    const int size1 = seq1.size(), size2 = seq2.size();
    bool state = true;
    ItemType val;
    while(i < size1 && j < size2)
    {
        if(state)
        {
            seq1.get(i, val);
            tmp.insert(k, val);
            i++;
            state = false;
        }
        else
        {
            seq2.get(j, val);
            tmp.insert(k, val);
            j++;
            state = true;
        }
        k++;
    }
    while(i < size1)
    {
        seq1.get(i, val);
        tmp.insert(k, val);
        k++;
        i++;
    }
    while(j < size2)
    {
        seq2.get(j, val);
        tmp.insert(k, val);
        k++;
        j++;
    }
    result = tmp;
}