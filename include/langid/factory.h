/** \file factory.h
 * Definition of class Factory.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Oct 30, 2009
 */

#ifndef LANGID_FACTORY_H
#define LANGID_FACTORY_H

namespace langid
{

class Analyzer;
class Knowledge;

/**
 * Factory creates instances for language identification.
 */
class Factory
{
public:
    /**
     * Create an instance of \e Factory.
     * \return the pointer to instance
     */
    static Factory* instance();

    /**
     * Create an instance of \e Analyzer.
     * \return the pointer to instance
     */
    virtual Analyzer* createAnalyzer();

    /**
     * Create an instance of \e Knowledge.
     * \return the pointer to instance
     */
    virtual Knowledge* createKnowledge();

protected:
    /**
     * Constructor.
     */
    Factory();

    /**
     * Destructor.
     */
    virtual ~Factory();

private:
    /** the instance of factory */
    static Factory* instance_;
};

} // namespace langid

#endif // LANGID_FACTORY_H
