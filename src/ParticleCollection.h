//
// Created by Caroline Cullen on 2019-05-16.
//

#ifndef PARTICLECOLLECTION_H
#define PARTICLECOLLECTION_H


class ParticleCollection {

public:

    vector<std::shared_ptr<Particle>> particles;
    GLuint ParticleVertexArrayID;
    int numP = 20;
    GLfloat points[1800];
    GLfloat pointColors[2400];
    GLuint particlePointsBuffer;
    GLuint particleColorBuffer;
    shared_ptr<Texture> particleTexture;
    float t = 0.0f;
    float h = 0.01f;
    glm::vec3 g = glm::vec3(0.0f, -0.01f, 0.0f);

    shared_ptr<Program> particleProg;

    ParticleCollection(){};

    void setUp()
    {
        particleProg = make_shared<Program>();
        particleProg->setVerbose(true);
        particleProg->setShaderNames(
                "../resources/particle_vert.glsl",
                "../resources/particle_frag.glsl");
        if (! particleProg->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        particleProg->addUniform("P");
        particleProg->addUniform("V");
        particleProg->addUniform("M");
        particleProg->addUniform("alphaTexture");
        particleProg->addAttribute("vertPos");
    }

    void initParticles()
    {
        int n = numP;

        for (int i = 0; i < n; ++ i)
        {
            auto particle = make_shared<Particle>();
            particles.push_back(particle);
        }
    }

    void updateParticles(bool* keyToggles, glm::vec3 player, ColorCollectGameplay* gameplay, float y)
    {
        for (auto particle : particles)
        {
            particle->update(t, h, g, keyToggles, player, gameplay);
        }
        t += h;

        auto temp = make_shared<MatrixStack>();
        temp->rotate(y, vec3(0, 1, 0));

        ParticleSorter sorter;
        sorter.C = temp->topMatrix();
        std::sort(particles.begin(), particles.end(), sorter);
    }

    void drawParticles(MatrixStack* View, float aspect, bool* keyToggles, glm::vec3 player, ColorCollectGameplay* gameplay, float y)
    {
        particleProg->bind();
        updateParticles(keyToggles, player, gameplay, y);

        auto Model = make_shared<MatrixStack>();
        Model->pushMatrix();
        Model->loadIdentity();

        auto Projection = make_shared<MatrixStack>();
        Projection->pushMatrix();
        Projection->perspective(45.0f, aspect, 0.01f, GROUND_SIZE);

        particleTexture->bind(particleProg->getUniform("alphaTexture"));
        CHECKED_GL_CALL(glUniformMatrix4fv(particleProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix())));
        CHECKED_GL_CALL(glUniformMatrix4fv(particleProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix())));
        CHECKED_GL_CALL(glUniformMatrix4fv(particleProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix())));

        CHECKED_GL_CALL(glEnableVertexAttribArray(0));
        CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, particlePointsBuffer));
        CHECKED_GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0));

        CHECKED_GL_CALL(glEnableVertexAttribArray(1));
        CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer));
        CHECKED_GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0));

        CHECKED_GL_CALL(glVertexAttribDivisor(0, 1));
        CHECKED_GL_CALL(glVertexAttribDivisor(1, 1));
        CHECKED_GL_CALL(glDrawArraysInstanced(GL_POINTS, 0, 1, numP));

        CHECKED_GL_CALL(glVertexAttribDivisor(0, 0));
        CHECKED_GL_CALL(glVertexAttribDivisor(1, 0));
        CHECKED_GL_CALL(glDisableVertexAttribArray(0));
        CHECKED_GL_CALL(glDisableVertexAttribArray(1));
        particleTexture->unbind();

        Model->popMatrix();
        particleProg->unbind();
    }

};


#endif // PARTICLECOLLECTION_H
