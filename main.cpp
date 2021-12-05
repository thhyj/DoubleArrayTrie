#include <iostream>
#include <vector>
#include <climits>
#include <cctype>

namespace DAT {
    template<typename T>
    struct AutoExpanseVector {
        using difference_type = typename std::vector<T>::difference_type;
        using size_type = typename std::vector<T>::size_type;

        std::vector<T> vector_;

        template<typename ... Args>
        inline void grow(size_type goal) {
            vector_.resize(std::max((vector_.size() << 1), (goal << 1)));
        }

        inline T &operator[](difference_type i) {
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

        DoubleArrayTrie() : pos_(1) {
            base_[ROOT_ID] = 1;
            check_[ROOT_ID] = 0;
        }


        void Insert(CharacterType *str, IdType size) {
            auto now_node_id = ROOT_ID;
            CharacterType *str_end = str + size;
            (*str_end++) = END_CHARACTER;
            for (CharacterType *str_left_start = str; str_left_start != str_end; ++str_left_start) {
                if (IsLeafNode(now_node_id, str_left_start + 1, str_end)) {
                    DealWithLeafNode(now_node_id, str_left_start, str_end);
                    break;
                }
                const CharacterType &ch = *str_left_start;
                auto new_node_id = base_[now_node_id] + ch;

                if (!IsCollision(now_node_id, new_node_id)) {// no collision;
                    DealWithNoCollision(now_node_id, new_node_id);
                    continue;
                } else { // collision !!!

                }

            }
        }

    private:
        AutoExpanseVector<IdType> base_, check_;
        AutoExpanseVector<CharacterType> tail_;
        IdType pos_;

        inline bool IsCollision(IdType prev_node_id, IdType new_node_id) {
            return !(IsNotUsedNodeId(new_node_id) || IsSameCheck(prev_node_id, new_node_id));
        }

        inline bool IsSameCheck(IdType prev_node_id, IdType new_node_id) {
            return check_[new_node_id] == prev_node_id;
        }

        inline bool IsNotUsedNodeId(IdType node_id) {
            return check_[node_id] == CHECK_EMPTY_VALUE;
        }

        inline bool IsLeafNode(IdType node_id) {
            return base_[node_id] == 0;
        }

        inline void DealWithNoCollision(IdType prev_node_id, IdType new_node_id) {
            check_[new_node_id] = prev_node_id;
        }

        inline void
        DealWithCollision(IdType prev_node_id, IdType new_node_id, CharacterType *str_begin, CharacterType *str_end) {
            if (check_[new_node_id] == prev_node_id) {
                if (IsStringEqual(str_begin, str_end, -base_[]))
            } else { // CASE 4 Conflict caused by position preemption

            }
        }

        inline void DealWithLeafNode(IdType node_id, CharacterType *str_begin, CharacterType *str_end) {
            if (str_begin >= str_end) {
                return;
            }
            IdType length = str_end - str_begin;
            if (base_[node_id] == 0) { // no collision
                for (IdType i = 0; i < length; ++i) {
                    tail_[pos_ + i] = str_begin[i];
                }
                pos_ += length;
                base_[node_id] = -pos_;
            } else { // CASE 3 Common prefix conflict
                IdType old_tail_start_pos = -base_[node_id];
                if(IsStringEqual(str_begin, str_end, old_tail_start_pos)) {
                    return;
                }

                for (IdType i = 0; i < length; ++i) {
                    if(str_begin[i] == tail_[old_tail_start_pos + i]) {
                        IdType new_node_candidate_id = FindFirstCandidate(str_begin[i]);

                    } else {

                    }
                }
            }
        }

        inline bool IsStringEqual(CharacterType *str_begin, CharacterType *str_end, IdType tail_start) {
            IdType length = str_end - str_begin;
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

        inline IdType FindFirstCandidate(std::vector<CharacterType> character_list) { // X_CHECK函数

        }

        inline IdType FindFirstCandidate(CharacterType character) { // X_CHECK函数

        }
    };
}

int main() {
    DAT::DoubleArrayTrie<char> a;
    return 0;
}
