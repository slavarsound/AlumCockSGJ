#include "Actors/Navigation/PatrolPath.h"

void APatrolPath::Reverse()
{
	Algo::Reverse(Waypoints);
}
