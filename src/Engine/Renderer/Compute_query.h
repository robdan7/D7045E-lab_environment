#pragma once
namespace Engine {

    /**
     * Compute queries are used to the engine, and preferably the GPU, to compute someting
     * and return a result.
     * @tparam Result_type
     * @tparam Input_params
     */
    template<typename Result_type, typename... Input_params>
    class Compute_query {
    public:
        virtual void execute(Input_params... params) = 0;
        virtual Result_type& fetch_result() = 0;
    };
}