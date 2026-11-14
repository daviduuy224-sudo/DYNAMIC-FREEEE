#include "../src/scoped_restore.hpp"
#include <cassert>
#include <map>
#include <stdexcept>
#include <vector>

struct UncopyableState {
    UncopyableState() = default;
    UncopyableState(UncopyableState const&) = delete;
    int mode = 1;
    void* portal = nullptr;
    std::map<int, std::vector<int>> activated{{1,{2}}};
};
int main() {
    UncopyableState s;
    int marker=0;
    auto candidate=[&](bool fail) {
        dynamic::ScopedRestore saved(s, &UncopyableState::mode,
            &UncopyableState::portal, &UncopyableState::activated);
        s.mode=5; s.portal=&marker; s.activated[1].push_back(3);
        s.activated[2]={7};
        if (fail) throw std::runtime_error("prediction interrupted");
        return;
    };
    for (bool fail : {false,true,false}) {
        try { candidate(fail); } catch (std::runtime_error const&) {}
        assert(s.mode==1 && !s.portal);
        assert(s.activated.size()==1 && s.activated.at(1)==std::vector<int>{2});
    }
}
