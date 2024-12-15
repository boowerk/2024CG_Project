#pragma once
class Panel
{
public:
	Panel() = default;

	virtual void Update() = 0;

	virtual ~Panel() = default;
};

