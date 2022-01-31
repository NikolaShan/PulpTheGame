//
//  PrefexOutputIterator.h
//  DaVinciQuiz
//
//  Created by mike on 15.05.14.
//
//

#ifndef DaVinciQuiz_PrefexOutputIterator_h
#define DaVinciQuiz_PrefexOutputIterator_h

template<typename T>
class PrefexOutputIterator {
    std::ostream&       ostream;
    std::string         prefix;
    bool                first;
public:
    
    typedef std::size_t                 difference_type;
    typedef T                           value_type;
    typedef T*                          pointer;
    typedef T                           reference;
    typedef std::output_iterator_tag    iterator_category;
    
    PrefexOutputIterator(std::ostream& o,std::string const& p = ""): ostream(o), prefix(p), first(true) {}
    
    PrefexOutputIterator& operator*()       {return *this;}
    PrefexOutputIterator& operator++()      {return *this;}
    PrefexOutputIterator& operator++(int)   {return *this;}
    
    void operator=(T const& value) {
        if (first)      {ostream << value;first = false;}
        else            {ostream << prefix << value;}
    }
};

#endif
