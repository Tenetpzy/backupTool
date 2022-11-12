#pragma once

#include <unordered_map>
#include <map>
#include <list>
#include <stdexcept>

class lru_error : public std::logic_error
{
public:
    explicit lru_error(const std::string &msg) : std::logic_error(msg) {}
};

template<typename T>
class lru_counter
{
public:
    void add_element(const T &ele)
    {
        if (ele_itr_map.count(ele) != 0)
            throw lru_error("add an exist element");
        
        add_element_inner(ele, 0);
    }

    void add_element_count(const T &ele)
    {
        if (ele_itr_map.count(ele) == 0)
            throw lru_error("add count: target element doesn't exist");
        size_t cnt = ele_itr_map[ele]->cnt;
        remove_element(ele);
        add_element_inner(ele, ++cnt);
    }

    T remove_less_use_entry()
    {
        if (ls.empty())
            throw lru_error("remove less use: element empty");
        T ele = ls.begin()->ele;
        remove_element(ele);
        return ele;
    }

private:

    // 保存元素与其使用计数，所有元素用有序链表inner_list连接
    template <typename U>
    struct inner_s
    {
        U ele;
        size_t cnt;

        inner_s(const U &e, size_t _cnt = 0)
            : ele(e), cnt(_cnt) {}
    };

    using inner_list = std::list<inner_s<T>>;
    using inner_itr = typename std::list<inner_s<T>>::iterator;

    inner_list ls;  // 有序链表，cnt从小到大，cnt相同的按访问时间从远到近
    std::unordered_map<T, inner_itr> ele_itr_map;   // element映射到其在list中的itr
    std::map<size_t, inner_itr> cnt_upper_itr_map;  // cnt映射到有序链表中值为cnt的upper元素的itr

    void remove_element(const T &ele)
    {
        auto itr = ele_itr_map[ele];
        const size_t cnt = itr->cnt;
        auto cnt_itr = cnt_upper_itr_map[cnt];
        if (itr != cnt_itr)   // 如果要移除的元素不是值为cnt的upper元素，直接移除即可
            ls.erase(itr);
        else  // 否则，cnt_itr也指向该元素，需要调整cnt_upper_itr_map表
        {
            auto is_only_cnt = [](inner_itr ls_begin, inner_itr itr) -> bool
            {
                if (itr == ls_begin)
                    return true;
                size_t cnt = itr->cnt;
                --itr;
                if (itr->cnt != cnt)
                    return true;
                return false;
            };

            // 如果该cnt在链表中唯一，则还需要移除cnt_upper_itr_map表中的cnt表项
            if (is_only_cnt(ls.begin(), cnt_itr))
                cnt_upper_itr_map.erase(cnt);
            // 否则，cnt_itr指向前一个值为cnt的元素
            else
                --cnt_upper_itr_map[cnt];
            
            ls.erase(itr);
        }

        ele_itr_map.erase(ele);
    }

    // 调整element的计数值为cnt时，内部调用
    // 调用时确保不存在element
    void add_element_inner(const T &ele, size_t ele_cnt)
    {
        auto cnt_map_itr = cnt_upper_itr_map.lower_bound(ele_cnt);
        inner_itr res_itr;

        // 若已经存在值为cnt的元素
        // 将其作为新的upper元素
        if (cnt_map_itr != cnt_upper_itr_map.end() && cnt_map_itr->second->cnt == ele_cnt)
        {
            inner_itr itr = cnt_map_itr->second;
            res_itr = ls.insert(++itr, inner_s<T>(ele, ele_cnt));
        }

        // 否则，不存在值为cnt的元素
        // 找到比它小的upper元素
        else
        {
            // 不存在比它小的upper元素
            // 插到链表头即可
            if (cnt_map_itr == cnt_upper_itr_map.begin())
            {
                ls.emplace_front(ele, ele_cnt);
                res_itr = ls.begin();
            }

            // 否则，插入到比它小的upper元素的后一个
            else
            {
                --cnt_map_itr;
                auto pos = cnt_map_itr->second;
                res_itr = ls.insert(++pos, inner_s<T>(ele, ele_cnt));
            }
        }

        cnt_upper_itr_map[ele_cnt] = res_itr;
        ele_itr_map[ele] = res_itr;
    }
    
};