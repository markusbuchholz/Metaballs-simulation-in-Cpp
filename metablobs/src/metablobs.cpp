// Markus Buchholz, 2023

#include <stdafx.hpp>
#include "imgui_helper.hpp"
#include <tuple>
#include <vector>
#include <math.h>
#include <random>
#include <algorithm>

//---------------------------------------------------------------

int w = 1000;
int h = 1000;

int size = 1;
int rows = 700;
int cols = 700;

float dt = 50.0f;
//---------------------------------------------------------------

int generateRandomAng()
{

	std::random_device engine;
	std::mt19937 generator(engine());
	std::uniform_int_distribution<int> distrib(1, 10);
	return distrib(generator);
}
//---------------------------------------------------------------

int generateRandomR()
{

	std::random_device engine;
	std::mt19937 generator(engine());
	std::uniform_real_distribution<float> distrib(6, 13);
	return distrib(generator) * 1000;
}
//---------------------------------------------------------------

float generateRandomPos()
{

	std::random_device device;
	std::mt19937 engine(device());
	std::uniform_real_distribution<float> dist(1.0, cols); // size

	return dist(engine);
}
//---------------------------------------------------------------

struct Blob
{

	float x;
	float y;
	float r;
	int sign_x;
	int sign_y;
	int ang_x;
	int ang_y;

	Blob(float x, float y, float r) : x(x), y(y), r(r)
	{
		sign_x = 1;
		sign_y = 1;
		ang_x = generateRandomAng();
		ang_y = generateRandomAng();
	}

	void move()
	{

		if (this->x < 0)
		{

			sign_x = 1;
		}

		if (this->x > cols)
		{
			sign_x = -1;
		}

		if (this->y < 0)
		{

			sign_y = 1;
		}

		if (this->y > rows)
		{
			sign_y = -1;
		}

		this->x = x + dt * sign_x * std::cos(M_PI / ang_x);
		this->y = y + dt * sign_y * std::sin(M_PI / ang_y);
	}
};

//---------------------------------------------------------------

struct RGB
{

	float r;
	float g;
	float b;
};

//---------------------------------------------------------------

struct HSV
{
	float h;
	float s;
	float v;
};

//---------------------------------------------------------------

int main(int argc, char const *argv[])
{

	std::string title = "MetaBalls";
	initImgui(w, h, title);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(89.0f / 255.0, 88.0f / 255.0, 87.0f / 255.0, 1.00f);

	bool flag = true;
	int B = 5;

	std::vector<Blob> blobs;

	for (int ii = 0; ii < B; ii++)
	{
		blobs.emplace_back(generateRandomPos(), generateRandomPos(), generateRandomR());
	}

	HSV hsv;
	RGB rgb;

	while (!glfwWindowShouldClose(window) && flag == true)
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		static int dots_nr = 15;

		ImGuiWindowFlags window_flags = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Simulation", nullptr, window_flags);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		for (auto &b : blobs)
		{

			b.move();
		}

		for (int ii = 0; ii < cols; ii++)
		{
			for (int jj = 0; jj < rows; jj++)
			{
				auto D = 0.0;
				ImVec2 p0;
				ImVec2 p1;
				for (auto &b : blobs)
				{
					p0 = {(float)ii, (float)jj};
					p1 = {(float)ii + 1, (float)jj + 1};

					float dist = sqrt(std::pow(b.x - p0.x, 2) + std::pow(b.y - p1.y, 2));
					auto c = 1.0 * b.r / dist;
					D += c;
				}

				hsv = {(float)D / 255.0f, 1.0f, 1.0f};
				ImGui::ColorConvertHSVtoRGB(hsv.h, hsv.s, hsv.v, rgb.r, rgb.g, rgb.b);

				ImVec4 pix_color = ImVec4(rgb.r, rgb.g, rgb.b, 1.0f);

				draw_list->AddRectFilled(p0, p1, ImColor(pix_color));
			}
		}


		// ImGui::PushItemWidth(300);
		// ImGui::SliderInt("dots", &dots_nr, 1, 100);
		// ImGui::PopItemWidth();
		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	termImgui();
	return 0;
}
