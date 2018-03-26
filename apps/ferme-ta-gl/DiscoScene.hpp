#include "Scene.hpp"

class DiscoScene : public Scene
{
public:
	DiscoScene(std::vector<std::shared_ptr<ObjectModel>>& models);
	void welcome(Camera& camera);
	void update(float elapsedTime, Camera& cam);
};