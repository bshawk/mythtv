#ifndef SCHEDULER_H_
#define SCHEDULER_H_

class ProgramInfo;
class QSqlDatabase;
class EncoderLink;

#include <qmap.h> 
#include <list>
#include <vector>
#include <qobject.h>
using namespace std;

class Scheduler : public QObject
{
  public:
    Scheduler(QMap<int, EncoderLink *> *tvList, QSqlDatabase *ldb);
   ~Scheduler();

    bool CheckForChanges(void);
    bool FillRecordLists(bool doautoconflicts = true);

    void RemoveRecording(ProgramInfo *pginfo);

    void RemoveFirstRecording(void); 
    ProgramInfo *GetNextRecording(void);

    list<ProgramInfo *> *getAllPending(void) { return &recordingList; }

    list<ProgramInfo *> *getConflicting(ProgramInfo *pginfo,
                                        bool removenonplaying = true,
                                        list<ProgramInfo *> *uselist = NULL);

    void PrintList(void);

    void customEvent(QCustomEvent *e);

  protected:
    void RunScheduler(void);
    static void *SchedulerThread(void *param);

  private:
    void setupCards(void);

    void MarkKnownInputs(void);
    void MarkConflicts(list<ProgramInfo *> *uselist = NULL);
    void PruneList(void);

    void MarkConflictsToRemove(void);
    void MarkSingleConflict(ProgramInfo *info,
                            list<ProgramInfo *> *conflictList);
    void CheckOverride(ProgramInfo *info, list<ProgramInfo *> *conflictList);
    void RemoveConflicts(void);
    void GuessSingle(ProgramInfo *info, list<ProgramInfo *> *conflictList);
    void GuessConflicts(void);

    bool Conflict(ProgramInfo *a, ProgramInfo *b);

    bool FindInOldRecordings(ProgramInfo *pginfo);      

    ProgramInfo *GetBest(ProgramInfo *info, 
                         list<ProgramInfo *> *conflictList);
    void DoMultiCard();

    list<ProgramInfo *> *CopyList(list<ProgramInfo *> *sourcelist);

    QSqlDatabase *db;

    list<ProgramInfo *> recordingList;

    bool hasconflicts;

    int numcards;
    int numsources;
    int numinputs;

    QMap<int, int> numInputsPerSource;
    QMap<int, vector<int> > sourceToInput;
    QMap<int, int> inputToCard;

    QMap<int, EncoderLink *> *m_tvList;   

    QMap<QString, bool> askedList;
    QMap<QString, bool> responseList;

    pthread_mutex_t schedulerLock;
};

#endif
