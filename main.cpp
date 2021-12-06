#include <iostream>
#include <vector>
#include <climits>
#include <cctype>
#include <map>
#include <set>
#include <cassert>
#include <cstring>
#include <optional>

namespace DAT {
    template<typename T>
    struct AutoExpanseVector {
        using difference_type = typename std::vector<T>::difference_type;
        using size_type = typename std::vector<T>::size_type;

        std::vector<T> vector_;

        template<typename ... Args>
        void grow(size_type goal) {
            vector_.resize(std::max((vector_.size() << 1), (goal << 1)));
        }

        size_type size() {
            return vector_.size();
        }

        T get(difference_type id) {
            return vector_[id];
        }

        T &operator[](difference_type i) {
            while (i >= vector_.size()) {
                grow(i);
            }
            return vector_[i];
        }
    };

    /**
     * Double Array Trie,
     * when insert same strings, only count once;
     * @tparam CharacterType
     * @tparam END_CHARACTER
     */
    template<typename CharacterType, CharacterType END_CHARACTER = '#', typename IdType = int32_t>
    class DoubleArrayTrie {
    public:
        constexpr static IdType ROOT_ID = 1;
        constexpr static IdType CHECK_EMPTY_VALUE = 0;
        constexpr static IdType NULL_ID = -1;

        DoubleArrayTrie() : pos_(1) {
            base_[ROOT_ID] = 1;
            check_[ROOT_ID] = 0;
            is_build_finish_ = false;
        }


        void FinishBuild() {
            is_build_finish_ = true;
            decltype(character_set_) temp_set;
            character_set_.swap(temp_set);
        }

        // as EndCharacter wiil be appended, one more sizeof(CharacterType) memory is required!
        void Insert(CharacterType *str, IdType size) {
            assert(!is_build_finish_);
            if (size <= 0) {
                return;
            }
            auto now_node_id = ROOT_ID;
            CharacterType *str_end = str + size;
            CharacterType origin_end = *str_end;
            (*str_end++) = END_CHARACTER;
            ++size;
            ExpandCharacterSet(str, size);

            for (CharacterType *str_left_start = str; str_left_start != str_end; ++str_left_start) {
                if (IsLeafNode(now_node_id)) {
                    DealWithLeafNode(now_node_id, str_left_start, str_end);
                    break;
                }

                const CharacterType &ch = *str_left_start;
                auto new_node_id = base_[now_node_id] + ch;
                if (now_node_id == new_node_id) {
                    new_node_id = DealWithCollisionToMyself(now_node_id, ch);
                    now_node_id = DealWithNoCollision(now_node_id, new_node_id);
                    continue;
                }
                if (!IsCollision(now_node_id, new_node_id)) {// no collision;
                    now_node_id = DealWithNoCollision(now_node_id, new_node_id);
                    continue;
                } else { // collision !!!
                    now_node_id = DealWithCollision(check_[new_node_id], now_node_id, ch);
                    new_node_id = base_[now_node_id] + ch;
                    now_node_id = DealWithNoCollision(now_node_id, new_node_id);
                }
            }
            (*--str_end) = origin_end;
        }
        // as EndCharacter wiil be appended, one more sizeof(CharacterType) memory is required!
        std::optional<IdType> Find(CharacterType *str, IdType size) {
            CharacterType *str_end = str + size;
            CharacterType origin_end = *str_end;
            (*str_end++) = END_CHARACTER;
            ++size;
            auto now_node_id = ROOT_ID;
            for (CharacterType *str_left_start = str; str_left_start != str_end; ++str_left_start) {
                if (IsLeafNode(now_node_id)) {
                    (*--str_end) = origin_end;
                    return FindWithLeafNode(now_node_id, str_left_start, str_end);
                } else {
                    const CharacterType &ch = *str_left_start;
                    const IdType next_node_id = base_[now_node_id] + ch;
                    if (check_[next_node_id] == now_node_id) {
                        now_node_id = next_node_id;
                    } else {
                        (*--str_end) = origin_end;
                        return std::nullopt;
                    }
                }
            }
            (*--str_end) = origin_end;
            return IsLeafNode(now_node_id) ? std::make_optional(now_node_id) : std::nullopt;
        }

        void Print() {
            static int times = 0;
            puts("---------------------------------------------------------------------");
            std::cout << "times = " << ++times << '\n';
            for (int i = 0; i < base_.size(); ++i) {
                if (base_[i] != 0) {
                    std::cout << "base[" << i << "] = " << base_[i] << '\n';
                }
            }
            puts("");
            for (int i = 0; i < check_.size(); ++i) {
                if (check_[i] != 0) {
                    std::cout << "check[" << i << "] = " << check_[i] << '\n';
                }
            }
            puts("");
            for (int i = 0; i < tail_.size(); ++i) {
                std::cout << "tail_[" << i << "] = " << (int) tail_[i] << '\n';
            }
            puts("---------------------------------------------------------------------");

        }

    private:
        AutoExpanseVector<IdType> base_, check_;
        AutoExpanseVector<CharacterType> tail_;
        IdType pos_;
        bool is_build_finish_;
        std::set<CharacterType> character_set_; // only can be used when building;

        void ExpandCharacterSet(CharacterType *str, IdType size) {
            CharacterType *end = str + size;
            for (; str != end; ++str) {
                character_set_.insert(*str);
            }
        }

        bool IsCollision(IdType prev_node_id, IdType new_node_id) {
            return !(IsNotUsedNodeId(new_node_id) || IsSameCheck(prev_node_id, new_node_id));
        }

        bool IsSameCheck(IdType prev_node_id, IdType new_node_id) {
            return check_[new_node_id] == prev_node_id;
        }

        bool IsNotUsedNodeId(IdType node_id) {
            return check_[node_id] == CHECK_EMPTY_VALUE;
        }

        bool IsLeafNode(IdType node_id) {
            return base_[node_id] <= 0;
        }

        void MoveToNewBase(IdType node_id,IdType old_base, IdType new_base, const std::vector<CharacterType> &sons) {
            for (const auto &son_character : sons) {
                const IdType old_son = old_base + son_character;
                const IdType new_son = new_base + son_character;
                base_[new_son] = base_[old_son];
                check_[new_son] = check_[old_son];

                if (!IsLeafNode(old_son)) {
                    const auto sons_of_son = GetAllSons(old_son);
                    for (const auto &son_of_son : sons_of_son) {
                        check_[son_of_son + base_[old_son]] = new_son;
                    }
                }

                base_[old_son] = 0;
                check_[old_son] = CHECK_EMPTY_VALUE;
            }
        }

        IdType DealWithCollisionToMyself(IdType node_id, CharacterType next_character) {
            std::vector<CharacterType> sons = GetAllSons(node_id);
            sons.template emplace_back(next_character);
            IdType new_base = FindFirstCandidate(sons);
            IdType old_base = base_[node_id];
            base_[node_id] = new_base;
            sons.pop_back();
            MoveToNewBase(node_id, old_base, new_base, sons);
            return base_[node_id] + next_character;
        }

        IdType DealWithNoCollision(IdType prev_node_id, IdType new_node_id) {
            check_[new_node_id] = prev_node_id;
            return new_node_id;
        }

        std::vector<CharacterType> GetAllSons(IdType node_id) { // cost a lot of time
            std::vector<CharacterType> sons;
            const IdType now_base = base_[node_id];
            for (const auto &character : character_set_) {
                if (check_[now_base + character] == node_id) {
                    sons.emplace_back(character);
                }
            }
            return sons;
        }

        IdType
        DealWithCollision(IdType prev_node_id, IdType next_node_id,
                          CharacterType next_character) { // CASE 4 Conflict caused by position preemption
            std::vector<CharacterType> sons_of_prev_node = GetAllSons(prev_node_id);
            std::vector<CharacterType> sons_of_new_node = GetAllSons(next_node_id);
            sons_of_new_node.template emplace_back(next_character);
            bool is_father_and_son = check_[next_node_id] == prev_node_id;

            const IdType yield_node_id =
                    sons_of_new_node.size() >= sons_of_prev_node.size() ? prev_node_id : next_node_id;
            const auto &sons_of_yield_node =
                    sons_of_new_node.size() >= sons_of_prev_node.size() ? sons_of_prev_node : sons_of_new_node;
            const IdType old_base = base_[yield_node_id];
            const IdType new_base_candidate = FindFirstCandidate(sons_of_yield_node);
            base_[yield_node_id] = new_base_candidate;
            if (check_[base_[next_node_id] + sons_of_new_node.back()] != next_node_id) {
                sons_of_new_node.pop_back();
            }
            MoveToNewBase(yield_node_id, old_base, new_base_candidate, sons_of_yield_node);
            if (is_father_and_son && yield_node_id == prev_node_id) {
                return next_node_id + new_base_candidate - old_base;
            }
            return next_node_id;
        }

        void SaveToTail(const std::vector<CharacterType> &str, IdType tail_start_pos) {
            for (IdType i = 0; i < str.size(); ++i) {
                tail_[tail_start_pos + i] = str[i];
            }
        }

        void SaveToTail(CharacterType *str_begin, CharacterType *str_end, IdType tail_start_pos) {
            if (str_begin >= str_end) {
                return;
            }
            const IdType length = str_end - str_begin;
            for (IdType i = 0; i < length; ++i) {
                tail_[tail_start_pos + i] = str_begin[i];
            }
        }


        std::optional<IdType> FindWithLeafNode(IdType node_id, CharacterType *str_begin, CharacterType *str_end) {
            if (str_begin >= str_end) {
                return node_id;
            }
            if (IsStringEqual(str_begin, str_end, -base_[node_id])) {
                return node_id;
            } else {
                return std::nullopt;
            }
        }

        void DealWithLeafNode(IdType node_id, CharacterType *str_begin, CharacterType *str_end) {
            if (str_begin >= str_end) {
                return;
            }
            const IdType length = str_end - str_begin;
            if (base_[node_id] == 0) { // no collision
                SaveToTail(str_begin, str_end, pos_);
                base_[node_id] = -pos_;
                pos_ += length;
            } else { // CASE 3 Common prefix conflict
                const IdType old_tail_start_pos = -base_[node_id];
                if (IsStringEqual(str_begin, str_end, old_tail_start_pos)) {
                    return;
                }

                for (IdType i = 0; i < length; ++i) {
                    if (str_begin[i] == tail_[old_tail_start_pos + i]) {
                        const auto &now_character = str_begin[i];
                        const IdType new_candidate_base = FindFirstCandidate(now_character);
                        base_[node_id] = new_candidate_base;
                        const IdType new_node_id = new_candidate_base + now_character;
                        check_[new_node_id] = node_id;
                        node_id = new_node_id;
                        continue;
                    } else {
                        const auto &now_character_new = str_begin[i];
                        const auto &now_character_prev = tail_[old_tail_start_pos + i];
                        const IdType new_candidate_base = FindFirstCandidate({now_character_new, now_character_prev});
                        base_[node_id] = new_candidate_base;

                        {
                            const IdType new_node_id_prev = new_candidate_base + now_character_prev;
                            base_[new_node_id_prev] = -old_tail_start_pos;
                            check_[new_node_id_prev] = node_id;
                            if (now_character_prev != END_CHARACTER) {
                                std::vector<CharacterType> temp_vector_saving_prev_left_str;
                                for (CharacterType *now_character = &tail_[old_tail_start_pos + i + 1];
                                     *now_character != END_CHARACTER; ++now_character) {
                                    temp_vector_saving_prev_left_str.emplace_back(*now_character);
                                }
                                temp_vector_saving_prev_left_str.template emplace_back(END_CHARACTER);
                                SaveToTail(temp_vector_saving_prev_left_str, old_tail_start_pos);
                            }
                        }

                        {
                            const IdType new_node_id_new = new_candidate_base + now_character_new;
                            base_[new_node_id_new] = -pos_;
                            check_[new_node_id_new] = node_id;
                            SaveToTail(str_begin + i + 1, str_end, pos_);
                            pos_ += str_end - (str_begin + i + 1);
                        }
                        return;
                    }
                }
            }
        }

        bool IsStringEqual(CharacterType *str_begin, CharacterType *str_end, IdType tail_start) {
            const IdType length = str_end - str_begin;
            for (IdType i = 0; i < length; ++i) {
                if (tail_[tail_start + i] == END_CHARACTER) {
                    return str_begin[i] == END_CHARACTER;
                }
                if (str_begin[i] != tail_[tail_start + i]) {
                    return false;
                }
            }
            return true;
        }

        IdType FindFirstCandidate(std::vector<CharacterType> character_list) { // X_CHECK函数
            for (IdType q = 1;; ++q) {
                for (const auto character : character_list) {
                    if (check_[q + character] != 0) {
                        goto next_round;
                    }
                }
                return q;
                next_round:
                continue;
            }
        }

        IdType FindFirstCandidate(CharacterType character) { // X_CHECK函数
            for (IdType q = 1;; ++q) {
                if (check_[q + character] == 0) {
                    return q;
                }
            }
        }
    };
}

template<typename CharacterType, typename IdType>
void TestFind(DAT::DoubleArrayTrie<CharacterType> &a, CharacterType *str, IdType size) {
    auto result = a.Find(str, size);
    if (!result) {
        std::cout << "Not found \n";
    } else {
        std::cout << "Found at id " << result.value() << '\n';
    }
}

void test1() {
    DAT::DoubleArrayTrie<char> a;
    std::vector<char> str1{3, 2, 4, 9, 6, 13, 16, 19}; //bachelor
    a.Insert(&str1[0], str1.size());
    a.Print();
    std::vector<char> str2{11, 2, 19}; //jar
    a.Insert(&str2[0], str2.size());
    a.Print();
    std::vector<char> str3{3, 2, 5, 8, 6}; //badge
    a.Insert(&str3[0], str3.size());
    a.Print();
    std::vector<char> str4{3, 2, 3, 26}; //baby
    a.Insert(&str4[0], str4.size());
    a.Print();
    a.FinishBuild();
    TestFind(a, &str1[0], str1.size());
    TestFind(a, &str2[0], str2.size());
    TestFind(a, &str3[0], str3.size());
    TestFind(a, &str4[0], str4.size());

    std::vector<char> str5{3, 2, 5, 8, 6, 7};
    std::vector<char> str6{3, 2, 5, 8};
    TestFind(a, &str5[0], str5.size());
    TestFind(a, &str6[0], str6.size());


};

void test2() {
    DAT::DoubleArrayTrie<char> a;
    std::vector<char> str1{3, 2}; //ba
    a.Insert(&str1[0], str1.size());
    a.Print();
    std::vector<char> str2{3, 2, 4}; //bac
    a.Insert(&str2[0], str2.size());
    a.Print();
    std::vector<char> str3{3, 6}; //be
    a.Insert(&str3[0], str3.size());
    a.Print();
    std::vector<char> str4{3, 2, 6, 6, 6}; //baeee
    a.Insert(&str4[0], str4.size());
    a.Print();
    a.FinishBuild();

    TestFind(a, &str1[0], str1.size());
    TestFind(a, &str2[0], str2.size());
    TestFind(a, &str3[0], str3.size());
    TestFind(a, &str4[0], str4.size());

}

DAT::DoubleArrayTrie<char> dat;

void test3() {
    //   freopen("in.in", "r", stdin);
    //   freopen("out.out", "w", stdout);
    int n;
    scanf("%d", &n);
    char name[55];
    for (int i = 0; i < n; ++i) {
        scanf("%s", name);
        dat.Insert(name, strlen(name));
    }
    dat.FinishBuild();

    int m;
    scanf("%d", &m);
    std::set<int> finded;
    for (int i = 0; i < m; ++i) {

        scanf("%s", name);
        auto ret = dat.Find(name, strlen(name));
        if (!ret) {
            puts("WRONG");
        } else {
            if (finded.find(ret.value()) == finded.end()) {
                finded.insert(ret.value());
                puts("OK");
            } else {
                puts("REPEAT");
            }
        }
    }
}

void test4() {
    DAT::DoubleArrayTrie<char> a;
    std::vector<char> str1{3, 3, 3}; //bbb
    a.Insert(&str1[0], str1.size());
    a.Print();
    std::vector<char> str2{3, 3}; //bb
    a.Insert(&str2[0], str2.size());
    a.Print();
    a.FinishBuild();
    TestFind(a, &str1[0], str1.size());
    TestFind(a, &str2[0], str2.size());
}

int main() {
    test3();
    return 0;
}
