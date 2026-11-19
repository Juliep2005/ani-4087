#include <cstddef>
#include <iostream>
#include <string>

class FauxMoteur
{
public:
    bool wait()
    {
        if (trameOuverte)
        {
            ++refus;
            return false;
        }
        return true;
    }

    bool begin()
    {
        if (trameOuverte)
        {
            ++refus;
            return false;
        }
        trameOuverte = true;
        ++debutsAcceptes;
        return true;
    }

    bool end()
    {
        if (!trameOuverte)
        {
            ++refus;
            return false;
        }
        trameOuverte = false;
        ++finsAcceptees;
        return true;
    }

    std::size_t debutsAcceptes = 0;
    std::size_t finsAcceptees = 0;
    std::size_t refus = 0;
    std::size_t images = 0;

    bool ouverte() const
    {
        return trameOuverte;
    }

private:
    bool trameOuverte = false;
};

class GardeTrame
{
public:
    explicit GardeTrame(FauxMoteur& session)
        : session(session)
    {
    }

    GardeTrame(const GardeTrame&) = delete;
    GardeTrame& operator=(const GardeTrame&) = delete;
    GardeTrame(GardeTrame&&) = delete;
    GardeTrame& operator=(GardeTrame&&) = delete;

    ~GardeTrame()
    {
        session.end();
    }

private:
    FauxMoteur& session;
};

int main()
{
    std::size_t count = 0;
    std::string chemins;
    if (!(std::cin >> count >> chemins))
    {
        return 0;
    }

    std::string cheminsAvecGarde = chemins;
    FauxMoteur sansGarde;
    for (std::size_t i = 0; i < count; ++i)
    {
        if (!sansGarde.wait())
        {
            if (chemins[i] == 'N' || chemins[i] == 'C')
            {
                chemins[i] = 'V';
            }
        }
        else
        {
            if (!sansGarde.begin())
            {
                chemins[i] = 'V';
            }
        }

        if (chemins[i] == 'N')
        {
            ++sansGarde.images;
        }
        else if (chemins[i] == 'C')
        {
            continue;
        }
        else if (chemins[i] == 'R')
        {
            break;
        }

        sansGarde.end();
    }

    FauxMoteur avecGarde;
    for (std::size_t i = 0; i < count; ++i)
    {
        if (!avecGarde.wait())
        {
            cheminsAvecGarde[i] = 'V';
        }
        else
        {
            if (!avecGarde.begin())
            {
                cheminsAvecGarde[i] = 'V';
            }
        }

        GardeTrame garde(avecGarde);
        if (cheminsAvecGarde[i] == 'N')
        {
            ++avecGarde.images;
        }
        else if (cheminsAvecGarde[i] == 'C')
        {
            continue;
        }
        else if (cheminsAvecGarde[i] == 'R')
        {
            break;
        }
    }

    std::cout << "SANS GARDE DEBUTS " << sansGarde.debutsAcceptes
              << " FINS " << sansGarde.finsAcceptees
              << " REFUS " << sansGarde.refus
              << " IMAGES " << sansGarde.images
              << " OUVERT " << (sansGarde.ouverte() ? "OUI" : "NON") << '\n';
    std::cout << "AVEC GARDE DEBUTS " << avecGarde.debutsAcceptes
              << " FINS " << avecGarde.finsAcceptees
              << " REFUS " << avecGarde.refus
              << " IMAGES " << avecGarde.images
              << " OUVERT " << (avecGarde.ouverte() ? "OUI" : "NON") << '\n';
    return 0;
}
