#ifndef VEGA_H
#define VEGA_H

#include <QtCore>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multiroots.h>
#include "ParseAndEval/evaluator.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>


class Vega : public QThread
{
    Q_OBJECT

public:
    typedef std::pair<std::string, std::string> string_pair;
    typedef std::pair<double, double> double_pair;
    typedef std::map<std::string, double> Osobnik;

private:
    // Parametry VEGA
    quint32 m_rozmiarPopulacji;
    quint32 m_maxLiczbaPokolen;
    double  m_prawdKrzyzowania;
    double  m_prawdMutacji;

    // Parametry ogólne
    std::map<std::string, double_pair> m_mLimits;
    std::map<std::string, double_pair> m_mLimitsFromInequalities;
    qreal m_time;

    // Źródła i kody - optcode
    std::list<std::string> m_sFunkcjaCeluSource;
    std::vector<string_pair> m_sOgraniczeniaSource;

    Evaluator m_sFunkcjaCeluCode;
    Evaluator m_sOgraniczeniaCode;

    static std::vector<std::string> m_vVars_f;
    static std::vector<std::string> m_vOuts_f;

    std::vector<Osobnik> m_vOsobniki;

    boost::mt19937 randomNumbergenerator;
    boost::random::uniform_real_distribution< > uniformDistribution;
    boost::variate_generator< boost::mt19937&, boost::random::uniform_real_distribution < > >
            generateUniformNumber;

    boost::random::normal_distribution< > normalDistribution;
    boost::variate_generator< boost::mt19937&, boost::random::normal_distribution < > >
            generateNormalNumber;

public:
    Vega();

    void run() Q_DECL_OVERRIDE {
        solve();
    }

    bool solve();
    bool computeOgraniczenia();
    void createPopulation();
    Evaluator &getFunkcjaCelu();
    Evaluator &getOgraniczenia();

    void setFunkcjaCelu(std::list<std::string> &funkcje);
    void setOgraniczenia(std::vector<string_pair> ograniczenia);

    void setRozmiarPopulacji(quint32 N);
    void setMaxLiczbaPokolen(quint32 T);
    void setPrawdKrzyzowania(double pc);
    void setPrawdMutacji(double pm);

    void setLimit(std::string var, double lower, double upper);
    void setAnimationTime(qreal);

    friend int root_f (const gsl_vector * x, void *params, gsl_vector * f);

signals:
    void animate(std::vector<Vega::Osobnik> *data, float offset);
    void solvingFinished();

private: // Methods
    bool find_root(Evaluator *ev, Osobnik &out);
    bool shrinkLimits(std::string &name, quint8 ineq, double limit);
    int checkOgraniczenia(Osobnik &osobnik);
    bool krzyzowanie(/*In*/Osobnik &i, Osobnik &j, /*Out*/Osobnik &k, Osobnik &l);
    void krzyzowanie(Vega::Osobnik &i, Vega::Osobnik &j, Vega::Osobnik &out);
    bool mutacja(/*In*/Osobnik &i, /*Out*/Osobnik &k);
    qreal quality(Osobnik &in);
    bool eval(Osobnik &inout);

};

// Potrzebne deklaracje
int print_state (size_t iter, gsl_multiroot_fsolver * s);
int root_f (const gsl_vector * x, void *params, gsl_vector * f);

#endif // VEGA_H
