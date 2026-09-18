#include "csvapplication.hh"
#include "csvwindow.hh"
#include <iostream>
#include <stdexcept>
#include <cstdlib>
extern CsvApplication app;
extern CsvWindow windows[];
static bool inspected = false, observedBusy = false;
static void require(bool ok, const char *why) { if(!ok) { std::cerr << "FAIL: " << why << std::endl; std::exit(1); } }
static Fl_Window *dialog() {
    for(auto *w=Fl::first_window();w;w=Fl::next_window(w))
        if(w->label() && std::string(w->label())=="Sort By") return w;
    return nullptr;
}
static void inspectBusy(void*) {
    auto *w=dialog();
    require(w && w->shown(), "sort dialog must remain visible during sorting");
    for(auto *other=Fl::first_window();other;other=Fl::next_window(other))
        require(!other->label() || std::string(other->label())!="Processing", "processing popup flashed");
    for(int i=0;i<w->children();++i) require(!w->child(i)->active(), "sort controls should be disabled");
    CsvApplication::doSortWinCB(nullptr, 0);
    require(w->shown(), "busy dialog must not close during sort");
    observedBusy=true;
}
static void interact(void*) {
    auto *w=dialog(); require(w, "missing sort dialog");
    Fl_Choice *column=nullptr,*type=nullptr;
    Fl_Button *button=nullptr;
    for(int i=0;i<w->children();++i) {
        auto *child=w->child(i);
        if(auto *choice=dynamic_cast<Fl_Choice*>(child)) {
            if(choice->x()==90) column=choice;
            if(choice->x()==470) type=choice;
        }
        if(auto *b=dynamic_cast<Fl_Button*>(child)) button=b;
    }
    require(column && type && button,"missing controls");
    require(type->value()==0,"initial numeric column");
    column->value(1); column->do_callback();
    require(type->value()==1,"switch to text must select String");
    type->value(2);
    column->value(0); column->do_callback();
    require(type->value()==0,"switch back must select Numeric");
    inspected=true;
    button->do_callback();
    Fl::add_timeout(0,inspectBusy);
}
static void cancel(void*) {
    auto *w=dialog(); require(w,"dialog must reopen");
    CsvApplication::doSortWinCB(nullptr,0);
}
int main() {
    const int index=app.createNewWindow();
    require(index==0,"unexpected window slot");
    auto &doc=windows[index];
    doc.table->resizeTable(12000,2);
    for(int r=0;r<12000;++r) {doc.table->setCell(std::to_string(12000-r),r,0);doc.table->setCell("text",r,1);}
    doc.updateTable();
    Fl::add_timeout(0.05,interact);
    app.sort(0);
    require(inspected && observedBusy,"interaction callbacks not executed");
    require(doc.table->getCell(0,0)=="1" && doc.table->getCell(11999,0)=="12000","numeric sort result");
    require(doc.isChanged(),"sorted document should be marked modified");
    Fl::add_timeout(0.05,cancel);
    app.sort(1);
    require(doc.table->getCell(0,0)=="1","cancel must preserve table");
    doc.win->hide();
    std::cout << "PASS: column type updates, busy state, no Processing popup, sorting, cancel/reopen" << std::endl;
}
