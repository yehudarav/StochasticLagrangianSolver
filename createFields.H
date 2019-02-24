
Info << endl << "Registering the velocity" << endl; 
label UFieldLabel = i_ModelManager.GetInterpolator().registerVectorField("U",CMisc::GLOBAL,dimVelocity);

Info << " Registered at label " << UFieldLabel << endl;

Info<< "Reading field U\n" << endl;
volVectorField& U(*i_ModelManager.GetInterpolator().GetInterpolatedVectorField(UFieldLabel));

// (50.109 197.727 14.0606) | 1443
//Info << i_ModelManager.GetInterpolator().GetVectorFieldInterpolation(UFieldLabel)->spatialInterpolator().interpolate(vector(50.109,197.727,14.0606),1443) << endl;


#include "readGravitationalAcceleration.H"

#include "createPhi.H"

singlePhaseTransportModel laminarTransport(U, phi);

dimensionedScalar rhoInfValue
(
    "rhoInf",
    dimDensity,
    laminarTransport
);

volScalarField rhoInf
(
    IOobject
    (
        "rho",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    mesh,
    rhoInfValue
);

autoPtr<incompressible::turbulenceModel> turbulence
(
    incompressible::turbulenceModel::New(U, phi, laminarTransport)
);

volScalarField mu
(
    IOobject
    (
        "mu",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    laminarTransport.nu()*rhoInfValue
);

word kinematicCloudName("kinematicCloud");
args.optionReadIfPresent("cloudName", kinematicCloudName);

Info<< "Constructing kinematicCloud " << kinematicCloudName << endl;
basicStochasticCloud kinematicCloud
(
    kinematicCloudName,
    rhoInf,
    U,
    mu,
    g,
    i_ModelManager
);

#include "createNonInertialFrameFields.H"
