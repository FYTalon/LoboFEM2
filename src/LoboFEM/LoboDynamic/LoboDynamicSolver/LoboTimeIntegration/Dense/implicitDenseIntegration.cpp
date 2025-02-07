#include "implicitDenseIntegration.h"
#include "LoboDynamic/LoboDynamic.h"

Lobo::ImplicitDenseIntegration::ImplicitDenseIntegration(KineticModel *kineticmodel_, int num_DOfs_, double damping_ratio_, double timestep_, int skip_steps_, int flags_) : LoboTimeIntegration(num_DOfs_, damping_ratio_, timestep_, skip_steps_, flags_)
{
    kineticmodel = kineticmodel_;
    kineticmodel->setTimeStep(timestep);
    opsovler = NULL;
}

Lobo::ImplicitDenseIntegration::~ImplicitDenseIntegration()
{
    delete opsovler;
}

void Lobo::ImplicitDenseIntegration::runXMLscript(pugi::xml_node &xml_node)
{
    if (xml_node.child("Optimization"))
    {
        pugi::xml_node modelnode = xml_node.child("Optimization");
        int maxiter = 100;
        double tol = 0.001;
        if (modelnode.attribute("maxiter"))
        {
            maxiter = modelnode.attribute("maxiter").as_int();
        }

        if (modelnode.attribute("tol"))
        {
            tol = modelnode.attribute("tol").as_double();
        }

        if (modelnode.attribute("method"))
        {
            if (strcmp(modelnode.attribute("method").as_string(),
                       "newton") == 0)
            {
                opsovler = new Lobo::NewtonDense(kineticmodel,maxiter,tol);
            }
        }
    }
}

void Lobo::ImplicitDenseIntegration::precompute()
{
    opsovler->precompute();
}

void Lobo::ImplicitDenseIntegration::stepFoward()
{
    q_1 = q;
    //q_vel_1 = q_vel;

    //solve kineticmodel
    kineticmodel->setKineticStatus(q_vel, q_1);
    opsovler->solve(&q);
    opsovler->getResult(&q);

    q_vel_1 = q_vel;
    q_vel = (q - q_1) / timestep;

    q_vel *= damping_ratio;
    step++;
}