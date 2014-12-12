#ifndef PROJECTLOCAL_H
#define PROJECTLOCAL_H

#include <QObject>

class ProjectLocal: public QObject
{
  public:
    enum PersonRole { Translator, Reviewer, Approver, Undefined };

    ProjectLocal(){}
    ~ProjectLocal(){}

    void setRole( PersonRole v ){mRole = v;}
    PersonRole role() const {return static_cast<PersonRole>(mRole);}

    void setFirstRun( bool v ){mFirstRun = v;}
    bool firstRun() const{return mFirstRun;}

  protected:

    // Personal
    int mRole;
    bool mFirstRun;

  private:
};

#endif

