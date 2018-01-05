#ifndef STASHCODES_H
#define STASHCODES_H

namespace stash
{
	const int uCompOfWindAfterTimestep=2;
	const int vCompOfWindAfterTimestep=3;
	const int thetaAfterTimestep=4;
	const int q=10;
	const int qcf = 12;
	const int blDepth = 25;
	const int seaIceFractionAfterTimestep=31;
	const int orography = 33;
	const int cloudNumberAfterTimestep=75;
	const int rainNumberAfterTimestep =76;
	const int rainThirdMomentAfterTimestep =77;
	const int iceNumberAfterTimestep =78;
	const int snowNumberAfterTimestep = 79;
	const int snowThirdMomentAfterTimestep = 80;
	const int graupelNumberAfterTimestep = 81;
	const int graupelThirdMomentAfterTimestep =82;
	const int tracer23Dust=83;
	const int tracer24Oceanic=84;
	const int tracer25Soot=85;
	const int tracer26VolcanicAsh=86;
	const int tracer27H2SO4=87;
	const int tracer28NH4_2SO2=88;
	const int tracer29Mineral=89;
	const int wCompOfWind=150;
	const int qcl = 254;
	const int bulkCloudFraction=266;
	const int bulkCloudFractionInEachLayer=266;
	const int rainAmount = 272;
	const int waterVapourMixingRatio = 391;
	const int pressureAtRhoLevelsAfterTimestep=407;
	const int pressureAtThetaLevelsAfterTimestep=408;
	const int surfacePressureAfterTimestep=409;
	const int dustDivision1MassMixingRatio=431;
	const int dustDivision2MassMixingRatio=432;
	const int dustDivision3MassMixingRatio=433;
	const int dustDivision4MassMixingRatio=434;
	const int dustDivision5MassMixingRatio=435;
	const int dustDivision6MassMixingRatio=436;
	const int temperatureIncrementSw = 1181;
	const int incomingSwFluxToa=1207;
	const int outgoingSwFluxToa=1208;
	const int directFlux=1230;
	const int diffuseFlux=1231;
	const int totalDownwardSurfaceSwFlux=1235;
	const int temperatureIncrementLw = 2181;
	const int netDownwardSurfaceLwFlux=2201;
	const int outgoingLwFluxToa=2205;
	const int downwardLwFluxSurface=2207;
	const int seaIceTemperatureAfterBoundaryLayer=3049;
	const int surfaceAndBoundaryLayerHeatFlux=3217;
	const int xCompOfSurfAndBlWindStress_N_per_m2=3219;
	const int yCompOfSurfAndBlWindStress_N_per_m2=3220;
	const int surfaceAndBoundaryLayerTotalMoistureFlux=3223;
	const int surfaceLatentHeatFlux=3234;
	const int stableBlIndicator= 3305;
	const int scOverStableBlIndicator= 3306;
	const int wellMixedBlIndicator= 3307;
	const int decoubledScNotOverCuIndicator= 3308;
	const int decoupledScOverCuIndicator= 3309;
	const int cumulusCappedBlIndicator= 3310;
	const int entrainmentRateSurfaceMl = 3362;
	const int entrainmentRateBl = 3363;
	const int specHumAfterLsPrecip=4010;
	const int largeScaleRainAmount=4202;
	const int cloudLiqWatAfterLsPrecip=4205;
	const int cloudIceWatAfterLsPrecip=4206;
	const int relHumAfterLsPrecip=4207;
	const int convCloudAmountOnEachModelLevel=5212;
	const int uCompOfGravWaveStress=6201;
	const int vCompOfGravWaveStress=6202;
	const int xCompOfGravWaveStress=6223;
	const int yCompOfGravWaveStress=6224;
	const int temperatureIncrementBlCloud = 9181;
	const int extralowCloudAmount = 9202;
	const int lowCloudAmount = 9203;
	const int medCloudAmount = 9204;
	const int highCloudAmount = 9205;
	const int temperatureIncrementAdvection = 12181;
	const int temperatureIncrementDiffusion = 13181;
	const int tempOnModelLevels_K=16004;
	const int tempOnThetaLevels_K=16004;
	const int pressureAtMeanSeaLevel=16222;
	const int totalDustConc_ug_per_m3=17257;
	const int geopotentialOnThetaLevels = 16201;
	const int geopotentialOnPressureLevels = 16202;
	const int geopotentialOnRhoLevels = 16255;


}

#endif
