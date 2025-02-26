#include <iostream>
#include<string>
#include"../include/svector/RangeTree.h"

class Interval
{
public:
    Interval(double min, double max, std::string name)
        :m_min(min), m_max(max), m_name(name)
    {

    }
    double getMin() const
    {
        return m_min;
    }
    double getMax() const
    {
        return m_max;
    }
    std::string getName() const
    {
        return m_name;
    }
private:
    double m_min;
    double m_max;
    std::string m_name;
};

int main()
{
    std::vector<Interval> intervals;
    intervals.push_back(Interval(0.0, 0.7, "interval0"));
    intervals.push_back(Interval(0.1, 0.3, "interval1"));
    intervals.push_back(Interval(0.2, 0.7, "interval2"));
    intervals.push_back(Interval(0.2, 0.6, "interval3"));
    intervals.push_back(Interval(0.2, 0.6, "interval4"));
    intervals.push_back(Interval(0.5, 0.8, "interval5"));
    intervals.push_back(Interval(0.9, 1.0, "interval6"));

    std::vector<sci::Boundary<double, Interval>> boundaries;
    for (auto& interval : intervals)
    {
        boundaries.push_back(sci::Boundary<double, Interval>{ interval.getMin(), &interval});
        boundaries.push_back(sci::Boundary<double, Interval>{ interval.getMax(), &interval});
    }
    

    sci::RangeTree<double, Interval> tree;
    tree.build(boundaries.begin(), boundaries.end());
    std::vector<double> points{ -1.5, -0.5, 0.0, 0.05, 0.15, 0.2, 0.25, 0.65, 0.7, 0.75, 0.85, 0.95, 1.5, 2.5 };
    for (auto& p : points)
    {
        auto intervals = tree.getRangesContaining(p);
        std::cout << "Point at " << p << " is contained in intervals ";
        for (auto& interval : intervals)
            std::cout << interval->getName() << " ";
        std::cout << std::endl;
    }
}
