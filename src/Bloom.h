//
// Created by Alexa Drenick on 2019-06-03.
//

#ifndef OBTAIN_BLOOM_H
#define OBTAIN_BLOOM_H

class Bloom
{
    GLuint frameBuf[2];
    GLuint texBuf[2];

    GLuint depthBuf;

    GLuint screenBuf[1];
    GLuint screenTexBuf[1];

    GLuint brightnessBuf[1];
    GLuint brightnessTexBuf[1];

    GLuint quad_VertexArrayID;
    GLuint quad_vertexbuffer;

    std::shared_ptr<Program> tex_prog;
    std::shared_ptr<Program> comb_prog;
    std::shared_ptr<Program> bright_prog;
    
public:

    void doBrightnessBuf()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, brightnessBuf[0]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexBuf[0]);

        // example applying of 'drawing' the FBO texture
        bright_prog->bind();
        glUniform1i(bright_prog->getUniform("sceneBuf"), 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);    //drawing a quad
        glDisableVertexAttribArray(0);
        bright_prog->unbind();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void blur()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bool horizontal = true;
        bool first = true;
        for (int i = 0; i <10; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, frameBuf[(i+1)%2]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //Blur(texBuf[i%2], horizontal);
            if (first)
            {
                Blur(brightnessTexBuf[i%2], horizontal);
                first = false;
            } else {
                Blur(texBuf[i%2], horizontal);
            }

            horizontal = !horizontal;
        }

        //glBindFramebuffer(GL_FRAMEBUFFER, getScreenBuf());
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void bloomPlz()
    {
        doBrightnessBuf();

        blur();

        combine();
    }

    void combine()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texBuf[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, screenTexBuf[0]);

        // example applying of 'drawing' the FBO texture
        comb_prog->bind();
        glUniform1i(comb_prog->getUniform("bloomBuf"), 0);
        glUniform1i(comb_prog->getUniform("sceneBuf"), 1);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);    //drawing a quad
        glDisableVertexAttribArray(0);
        comb_prog->unbind();
    }

    void init(int width, int height)
    {
        initQuad();

        initTexProg();

        initCombProg();

        initBrightProg();

        glGenFramebuffers(1, screenBuf);
        glGenTextures(1, screenTexBuf);
        glGenRenderbuffers(1, &depthBuf);
        createFBO(screenBuf[0], screenTexBuf[0], width, height);

        glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(1, brightnessBuf);
        glGenTextures(1, brightnessTexBuf);
        createFBO(brightnessBuf[0], brightnessTexBuf[0], width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(2, frameBuf);
        glGenTextures(2, texBuf);
        //glGenRenderbuffers(1, &depthBuf);
        createFBO(frameBuf[0], texBuf[0], width, height);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //more FBO set up
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers);

        //create another FBO so we can swap back and forth
        createFBO(frameBuf[1], texBuf[1], width, height);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    GLuint getScreenBuf()
    {
        return screenBuf[0];
    }

    /* To call the blur on the specificed texture */
    void Blur(GLuint inTex, bool horizontal) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inTex);

        // example applying of 'drawing' the FBO texture
        tex_prog->bind();
        glUniform1i(tex_prog->getUniform("texBuf"), 0);
        glUniform1i(tex_prog->getUniform("horizontal"), horizontal);
        //glUniform1f(tex_prog->getUniform("time"), glfwGetTime());
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);    //drawing a quad
        glDisableVertexAttribArray(0);
        tex_prog->unbind();
    }

    void initTexProg()
    {
        //set up the shaders to blur the FBO decomposed just a placeholder pass thru now
        //TODO - modify and possibly add other shaders to complete blur
        tex_prog = make_shared<Program>();
        tex_prog->setVerbose(true);
        tex_prog->setShaderNames("../resources/blur_vert.glsl", "../resources/blur_frag.glsl");
        if (! tex_prog->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        tex_prog->addUniform("texBuf");
        tex_prog->addUniform("horizontal");
        tex_prog->addAttribute("vertPos");
    }

    void initCombProg()
    {
        //set up the shaders to blur the FBO decomposed just a placeholder pass thru now
        //TODO - modify and possibly add other shaders to complete blur
        comb_prog = make_shared<Program>();
        comb_prog->setVerbose(true);
        comb_prog->setShaderNames("../resources/blur_vert.glsl", "../resources/comb_frag.glsl");
        if (! comb_prog->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        comb_prog->addUniform("bloomBuf");
        comb_prog->addUniform("sceneBuf");
        comb_prog->addAttribute("vertPos");
    }

    void initBrightProg()
    {
        //set up the shaders to blur the FBO decomposed just a placeholder pass thru now
        //TODO - modify and possibly add other shaders to complete blur
        bright_prog = make_shared<Program>();
        bright_prog->setVerbose(true);
        bright_prog->setShaderNames("../resources/blur_vert.glsl", "../resources/brightness_frag.glsl");
        if (! bright_prog->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        bright_prog->addUniform("sceneBuf");
        bright_prog->addAttribute("vertPos");
    }

    void initQuad() {

        //now set up a simple quad for rendering FBO
        glGenVertexArrays(1, &quad_VertexArrayID);
        glBindVertexArray(quad_VertexArrayID);

        static const GLfloat g_quad_vertex_buffer_data[] = {
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                1.0f,  1.0f, 0.0f,
        };

        glGenBuffers(1, &quad_vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

    }

    /*
    Helper function to create the framebuffer object and associated texture to write to
    */
    void createFBO(GLuint& fb, GLuint& tex, int width, int height) {
        //initialize FBO (global memory)

        //set up framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, fb);
        //set up texture
        glBindTexture(GL_TEXTURE_2D, tex);

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            cout << "Error setting up frame buffer - exiting" << endl;
            exit(0);
        }
    }
};


#endif //OBTAIN_BLOOM_H
