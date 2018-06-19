#include "vega.h"
#include <ctime>
#include <QMessageBox>
#include <QApplication>


std::vector<std::string> Vega::m_vVars_f;
std::vector<std::string> Vega::m_vOuts_f;


Vega::Vega():
    randomNumbergenerator(time(0)),
    uniformDistribution(0.0, 1.0),
    generateUniformNumber( randomNumbergenerator, uniformDistribution ),
    normalDistribution(0.0, 1.0),
    generateNormalNumber(randomNumbergenerator, normalDistribution)
{
}

bool Vega::solve()
{
    for (auto &s : m_vOsobniki)
        eval(s);
    emit animate(&m_vOsobniki, 0.5);
    QThread::usleep(m_time/(m_maxLiczbaPokolen+4)*1000000);

    std::vector<Osobnik> A;

    // Przygotowanie generatora

    for (int pokolenie=0; pokolenie<m_maxLiczbaPokolen; pokolenie++) {
        std::vector<Osobnik> p;
        std::vector<Osobnik> pp;
        std::vector<Osobnik> ppp;
        quint32 N = m_rozmiarPopulacji;

        p = m_vOsobniki;

        // Krok 1 - Wyznaczenie dopasowania i selekcja
        for (auto &s : p) {
            eval(s);
        }

        // Krok 2 - Rekombinacja
        quint32 j = (N/2);
        for (quint32 i=0; i<(N/2); i++) {
            bool krzyzuj=false;
            // i,j na k,l - na podst prawdopodobieństwa
            Osobnik k,l;
            qreal prop = generateUniformNumber();
            if (prop <= m_prawdKrzyzowania) {
                try {
                krzyzowanie(p[i], p[j], k, l);
                } catch(...) {}
                qreal i_quality = quality(p[i]);
                qreal j_quality = quality(p[j]);
                qreal ij_quality = qSqrt(qPow(i_quality, 2) + qPow(j_quality, 2));

                if ( (checkOgraniczenia(k) == 0) && (checkOgraniczenia(l) == 0) ) {
                    eval(k);
                    eval(l);
                    qreal k_quality = quality(k);
                    qreal l_quality = quality(l);
                    qreal kl_quality = qSqrt(qPow(k_quality, 2) + qPow(l_quality, 2));

                    if (ij_quality > kl_quality) {
                        krzyzuj = true;
                    }
                }
            }

            if (krzyzuj) {
                pp.push_back(k);
                pp.push_back(l);
            } else {
                pp.push_back(p[i]);
                pp.push_back(p[j]);
            }
            j++;
        }


        // Krok 3 - Mutacja
        for (quint32 i=0; i<N; i++) {
            // i - na podst prawdopodobieństwa
            Osobnik k;
            bool krzyzuj=false;

            qreal prop = generateUniformNumber();
            if (prop <= m_prawdMutacji) {
                try {
                mutacja(pp[i], k);
                } catch(...) {}
                if (checkOgraniczenia(k) == 0) {
                    eval(k);
                    qreal i_quality = quality(pp[i]);
                    qreal k_quality = quality(k);
                    if (i_quality > k_quality) {
                        krzyzuj = true;
                    }
                }
            }

            if(krzyzuj) {
                ppp.push_back(k);
            } else {
                ppp.push_back(pp[i]);
            }
        }

        // Krok 4 - rozwiązania niezdominowane?
        m_vOsobniki = ppp;

        double min=999999999.999;
        double max=-999999999.999;
        for (auto &s: ppp) {
            if (s["f1"]<min) min = s["f1"];
            if (s["f1"]>max) max = s["f1"];
        }

        emit animate(&m_vOsobniki, 0.5);
        QThread::usleep(m_time/(m_maxLiczbaPokolen+4)*1000000);

        if (max-min<0.01) break;
    }

//    std::sort(A.begin(), A.end(), compareOsobniki);



    emit solvingFinished();
    return true;
}

bool Vega::computeOgraniczenia()
{
    std::vector<std::string> Outs_f;
    std::vector<std::string> Vars_f;

    for (auto &s : m_sOgraniczeniaCode.getFormula().variables()) {
        if (s.first[0]=='y') {
            Outs_f.push_back(s.first);
        } else {
            Vars_f.push_back(s.first);
        }
    }

    for (int i=0; i<Vars_f.size(); i++) {
        std::list<std::string> kod;

        for (auto &s : m_sOgraniczeniaSource) {
            if (s.first.find(Vars_f.at(i))==std::string::npos) {
                continue;
            }
            Evaluator ograniczenie;
            std::list<std::string> linia{s.first};
            ograniczenie.compile(linia);
            Osobnik osobnik;
            m_vVars_f.clear();
            m_vOuts_f.clear();
            m_vVars_f.push_back(Vars_f.at(i));

            for (auto &o : Outs_f) {
                if (linia.front().find(o)!=std::string::npos) {
                    m_vOuts_f.push_back(o);
                    break;
                }
            }

            if (find_root(&ograniczenie, osobnik)) {
                for (auto &v : m_vVars_f) {
                    if (s.second[0]=='>' && ograniczenie.getFormula().variables().size()>3) break;
                    shrinkLimits(v, s.second[0], osobnik.at(v));
                }
            } else {
                qDebug() << "Ograniczenia nie zostały rozwiązane";
            }
        }
    }
}

void Vega::createPopulation()
{
    m_vOsobniki.clear();
    int zabezpieczenie = 50*m_rozmiarPopulacji;

    for (int i=0; i<m_rozmiarPopulacji && zabezpieczenie; i++, zabezpieczenie--) {
        Osobnik out;
        for (auto &s : m_mLimitsFromInequalities) {
            out[s.first] = s.second.first;
        }
        int limit = 50;
        for (auto &s : m_mLimitsFromInequalities) {
            double left = s.second.first;
            double right = s.second.second;
            int checkOgr;
            limit = 50;
            while (--limit) {
                double size = right - left;

                if (size < 0.001) break;

                out[s.first] = size*generateUniformNumber( ) + left;

                checkOgr = checkOgraniczenia(out);
                if (checkOgr==0) {
                    break;
                } else {
                    if (checkOgr==-1) {
                        left = out[s.first];
                    } else {
                        right = out[s.first];
                    }
                }
            }
            if (limit <= 0) {
                break;
            }
            try {
                m_sOgraniczeniaCode.getFormula().variables().at(s.first).get() = out[s.first];
            } catch (...) {}
        }
        if (limit <= 0) {
            i--;
            continue;
        }

        // tutaj jeszcze sprawdzenie czy zgadzają się ograniczenia
        m_sOgraniczeniaCode.getFormula().eval();

        m_vOsobniki.push_back(out);
    }

    if (zabezpieczenie==0) {
        QMessageBox::warning(NULL, "Vega", "Nie poprawne ograniczenia! Limit", QMessageBox::Abort);
        throw std::string("Nie poprawne ograniczenia!");
    }
}

Evaluator &Vega::getFunkcjaCelu()
{
    return m_sFunkcjaCeluCode;
}

Evaluator &Vega::getOgraniczenia()
{
    return m_sOgraniczeniaCode;
}

void Vega::setFunkcjaCelu(std::list<std::string> &funkcje)
{
    m_sFunkcjaCeluSource = funkcje;
    m_sFunkcjaCeluCode.compile(m_sFunkcjaCeluSource);
}

void Vega::setOgraniczenia(std::vector<Vega::string_pair> ograniczenia)
{
    m_sOgraniczeniaSource = ograniczenia;
    std::list<std::string> kod;
    for (auto &s : m_sOgraniczeniaSource)
        kod.push_back(s.first);
    m_sOgraniczeniaCode.compile(kod);

    m_mLimitsFromInequalities = m_mLimits;
}

void Vega::setRozmiarPopulacji(quint32 N)
{
    m_rozmiarPopulacji = N;
}

void Vega::setMaxLiczbaPokolen(quint32 T)
{
    m_maxLiczbaPokolen = T;
}

void Vega::setPrawdKrzyzowania(double pc)
{
    m_prawdKrzyzowania = pc;
}

void Vega::setPrawdMutacji(double pm)
{
    m_prawdMutacji = pm;
}

void Vega::setLimit(std::string var, double lower, double upper)
{
    m_mLimits[var] = double_pair{lower,upper};
}

void Vega::setAnimationTime(qreal t)
{
    m_time = t;
}

bool Vega::find_root(Evaluator *ev, Osobnik &out)
{
    const gsl_multiroot_fsolver_type *T;
    gsl_multiroot_fsolver *s;

    int status;
    size_t i, iter = 0;
    const size_t n = m_vVars_f.size(); // Tutaj liczba sterowanych danych!
    gsl_multiroot_function f = {&root_f, n, ev};

    ev->getFormula().clearVariables(0);

    double x_init[n] = {1.0};
    gsl_vector *x = gsl_vector_alloc (n);

    for (quint32 it=0; it < n; it++) {
        gsl_vector_set (x, it, x_init[it]);
    }

    T = gsl_multiroot_fsolver_hybrids;
    s = gsl_multiroot_fsolver_alloc (T, m_vOuts_f.size());
    gsl_multiroot_fsolver_set (s, &f, x);

    do
    {
        iter++;
        status = gsl_multiroot_fsolver_iterate (s);

        if (status)   /* check if solver is stuck */
            break;

        status = gsl_multiroot_test_residual (s->f, 1e-3);
    } while (status == GSL_CONTINUE && iter < 1000);

    gsl_multiroot_fsolver_free (s);
    gsl_vector_free (x);

    if (status != GSL_SUCCESS) {
        return false;
    }

    for (auto &s : ev->getFormula().variables())
        if (s.first[0]!='y')
            out[s.first] = s.second;

    return true;
}

bool Vega::shrinkLimits(std::string &name, quint8 ineq, double limit)
{
    try {
        switch(ineq) {
        case '<':
            if (m_mLimitsFromInequalities[name].first < limit)
                m_mLimitsFromInequalities[name].first = limit;
            break;
        case '>':
            if (m_mLimitsFromInequalities[name].second > limit)
                m_mLimitsFromInequalities[name].second = limit;
            break;
        }
    } catch(...) {

    }
}

int Vega::checkOgraniczenia(Vega::Osobnik &osobnik)
{
    client::code_gen::Formula::VariableMap &mapa = m_sOgraniczeniaCode.getFormula().variables();
    m_sOgraniczeniaCode.getFormula().clearVariables();
    for (auto &s : osobnik) {
        if (s.first[0]=='f') continue;
        try {
            mapa.at(s.first).get() = s.second;
        } catch (...) {}
    }

    m_sOgraniczeniaCode.getFormula().eval();

    for (auto &s : mapa) {
        if (s.first[0]=='y') {
            switch (m_sOgraniczeniaSource[s.first[1]-'0'].second[0]) {
            case '<':
                if (s.second < -0.001) {
                    return -1;
                }
                break;
            case '>':
                if (s.second > 0.001) {
                    return 1;
                }
                break;
            }
        }
    }

    return 0;
}

void Vega::krzyzowanie(Vega::Osobnik &i, Vega::Osobnik &j, Vega::Osobnik &out) {
    for (auto &s: i) {
        qreal z1czy2 = generateUniformNumber();
        if (z1czy2<0.334) {
            out[s.first] = s.second;
        } else if (z1czy2<0.667) {
            out[s.first] = j[s.first];
        } else {
            out[s.first] = (s.second + j[s.first])*0.5;
        }
    }
}

bool Vega::krzyzowanie(Vega::Osobnik &i, Vega::Osobnik &j, Vega::Osobnik &k, Vega::Osobnik &l)
{
    if (i.size() == j.size()) {
        int zabezpieczenie = 100;
        do  {
            krzyzowanie(i, j, k);
            if (--zabezpieczenie<=0) {
                throw std::string("krzyzowanie nie powodzi się");
            }
        } while (checkOgraniczenia(k)!=0);

        do {
            krzyzowanie(i, j, l);
            if (--zabezpieczenie<=0) {
                throw std::string("krzyzowanie nie powodzi się");
            }
        } while (checkOgraniczenia(l)!=0);
    } else throw std::string("Osobniki nie zgadzają się wielkością. To nie powinno się zdarzyć!");
}

bool Vega::mutacja(Vega::Osobnik &i, Vega::Osobnik &k)
{
    static const qreal partOfLength = 0.1;
    int zabezpieczenie = 100;

    do {
        for (auto &s: i) {
            qreal left = m_mLimitsFromInequalities[s.first].first;
            qreal right = m_mLimitsFromInequalities[s.first].second;
            qreal size = right - left;
            qreal mutation = (size*partOfLength)*generateUniformNumber( );

            k[s.first] = k[s.first] + mutation;
        }
        if (--zabezpieczenie<=0) {
            throw std::string("mutacja nie powodzi się");
        }
    } while (checkOgraniczenia(k)!=0);
}

qreal Vega::quality(Vega::Osobnik &in)
{
    return in["f1"] + in["f2"];//qMin(in["f1"], in["f2"]);//qSqrt(qPow(in["f1"], 2) + qPow(in["f2"], 2));
}

bool Vega::eval(Vega::Osobnik &inout)
{
    for (auto &var : m_sFunkcjaCeluCode.getFormula().variables())
        if (var.first[0]!='f')
            var.second.get() = inout[var.first];

    m_sFunkcjaCeluCode.getFormula().eval();

    for (auto &var : m_sFunkcjaCeluCode.getFormula().variables())
        if (var.first[0]=='f')
            inout[var.first] = var.second;
}

int root_f (const gsl_vector * x, void *params, gsl_vector * f)
{
    client::code_gen::Formula::VariableMap &vars = ((Evaluator *) params)->getFormula().variables();

    for (quint32 it=0; it < Vega::m_vVars_f.size(); it++) {
        vars.at(Vega::m_vVars_f[it]).get() = gsl_vector_get (x, it);
    }

    ((Evaluator *) params)->getFormula().eval();

    for (quint32 it=0; it < Vega::m_vOuts_f.size(); it++) {
        gsl_vector_set (f, it, vars.at(Vega::m_vOuts_f[it]));
    }

    return GSL_SUCCESS;
}
