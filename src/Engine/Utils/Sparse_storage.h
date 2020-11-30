#pragma once

#include "Sparse_set.h"
namespace Engine {

    /**
     * Extension of the sparse set class. This also stores data in the same format as
     * the packed vector in a sparse set.
     * @tparam ID_type
     * @tparam Storage_type
     */
    template<typename ID_type, typename Storage_type>
    class Sparse_storage : public Sparse_set<ID_type>{
    public:
        Sparse_storage(uint32_t page_size_indexed) : m_packed_storage(std::vector<Storage_type>()) , Sparse_set<ID_type>(page_size_indexed) {
        }

        template <typename... Args>
        ID_type emplace(Args&&... args) {
            auto id = this->create_ID();
            Sparse_set<ID_type>::put(id);


            /// https://en.cppreference.com/w/cpp/types/is_aggregate
            /// The is_aggregate function wont work unless it is checked at compile time,
            /// hence the constexpr statement.
            //if constexpr (std::is_aggregate_v<Storage_type>) {
            //    this->m_packed_storage.push_back(Storage_type{std::forward<Args>(args)...});
            //} else {
                this->m_packed_storage.emplace_back(std::forward<Args>(args)...);
            //}
            return id;
        }

        ID_type push(Storage_type type) {
            auto id = this->create_ID();
            Sparse_set<ID_type>::put(id);


            /// https://en.cppreference.com/w/cpp/types/is_aggregate
            /// The is_aggregate function wont work unless it is checked at compile time,
            /// hence the constexpr statement.
            //if constexpr (std::is_aggregate_v<Storage_type>) {
                this->m_packed_storage.push_back(type);
            //} else {
            //this->m_packed_storage.emplace_back(std::forward<Args>(args)...);
            //}
            return id;
        }

        void remove(ID_type id) {
            auto index = this->index(id);
            if (index != this->m_packed_storage.size()-1) {
                this->m_packed_storage.at(this->index(id)) = this->m_packed_storage.back();
            }
            this->m_packed_storage.pop_back();
            //Sparse_set<ID_type>::remove(id);
            //this->remove(id);
            this->delete_ID(id);
        }

        Storage_type& get(ID_type ID) {
            return this->m_packed_storage.at(Sparse_set<ID_type>::index(ID));
        }

        auto get_sparse_list() {
            return &this->sparse_list;
        }

    private:
        ID_type create_ID() {
            //CORE_ASSERT(m_used_IDs.size() == (NULL_ID-1), "The maximum number of ECS entities has been reached! Total: [0}", (NULL_ID-1));

            if (m_last_deleted != NULL_ID) {
                /// We need to reuse an old entity ID.
                ID_type ID = m_last_deleted;
                m_last_deleted = m_used_IDs[m_last_deleted];
                m_used_IDs[ID] = ID;
                return ID;
            }
            return m_used_IDs.emplace_back(m_used_IDs.size());
        }
        void delete_ID(ID_type id) {
            //CORE_ASSERT(id != NULL_ID, "Cannot delete null pointer entity!");
            if (id == NULL_ID) {
                throw "ECS entity is equal to null pointer!";
            }

            Sparse_set<ID_type>::remove(id);
            m_used_IDs.at(id) = m_last_deleted;
            m_last_deleted = id;
        }
        std::vector<Storage_type> m_packed_storage;
        std::vector<ID_type> m_used_IDs;   /// List of all used entity IDs.
        const ID_type NULL_ID = std::numeric_limits<ID_type>::max();    /// Reserved null pointer.
        ID_type m_last_deleted = NULL_ID;
    };
}