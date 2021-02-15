/**
* @file Font.cpp
*/
#include "Font.h"
#include "Log.h"
#include <memory>
#include <iostream>
#include <stdio.h>

/**
* �t�H���g�`��@�\���i�[���閼�O���.
*/
namespace Font {

/**
* �t�H���g�p���_�f�[�^�^.
*/
struct Vertex
{
  glm::vec3 position;
  glm::u16vec2 uv;
  glm::u8vec4 color;
  glm::u8vec4 subColor;
  glm::u16vec2 thicknessAndOutline;
};

/**
* �t�H���g�`��I�u�W�F�N�g������������.
*
* @param maxChar �ő�`�敶����.
* @param screen  �`���X�N���[���̑傫��.
*
* @retval true  ����������.
* @retval false ���������s.
*/
bool Renderer::Init(size_t maxChar, const glm::vec2& screen)
{
  if (maxChar > (USHRT_MAX + 1) / 4) {
    LOG("WARNING: %d�͐ݒ�\�ȍő啶�������z���Ă��܂�.\n", maxChar);
    maxChar = (USHRT_MAX + 1) / 4;
  }
  vboCapacity = static_cast<GLsizei>(4 * maxChar);
  vbo.Init(GL_ARRAY_BUFFER, sizeof(Vertex) * vboCapacity, nullptr, GL_STREAM_DRAW);
  {
    std::vector<GLushort> tmp;
    tmp.resize(maxChar * 6);
    GLushort* p = tmp.data();
    for (GLushort i = 0; i < maxChar * 4; i += 4) {
      for (GLshort n : { 0, 1, 2, 2, 3, 0 }) {
        *(p++) = i + n;
      }
    }
    ibo.Init(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6 * maxChar, tmp.data(), GL_STATIC_DRAW);
  }
  vao.Init(vbo.Id(), ibo.Id());
  vao.VertexAttribPointer(vbo.Id(), 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, position));
  vao.VertexAttribPointer(vbo.Id(), 1, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex), offsetof(Vertex, uv));
  vao.VertexAttribPointer(vbo.Id(), 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), offsetof(Vertex, color));
  vao.VertexAttribPointer(vbo.Id(), 3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), offsetof(Vertex, subColor));
  vao.VertexAttribPointer(vbo.Id(), 4, 2, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(Vertex), offsetof(Vertex, thicknessAndOutline));

  progFont = Shader::Program::Create("Res/Shader/Font.vert", "Res/Shader/Font.frag");
  if (!progFont) {
    return false;
  }

  reciprocalScreenSize = 2.0f / screen;
  return true;
}

/**
* �t�H���g�t�@�C����ǂݍ���.
*
* @param filename �t�H���g�t�@�C����.
*
* @retval true  �ǂݍ��ݐ���.
* @retval false �ǂݍ��ݎ��s.
*/
bool Renderer::LoadFromFile(const char* filename)
{
  const std::unique_ptr<FILE, decltype(&fclose)> fp(fopen(filename, "r"), fclose);
  if (!fp) {
    return false;
  }

  int line = 1;
  float fontSize;
  int ret = fscanf(fp.get(), "info face=%*s size=%f bold=%*d italic=%*d charset=%*s"
    " unicode=%*d stretchH=%*d smooth=%*d aa=%*d padding=%*d,%*d,%*d,%*d spacing=%*d,%*d", &fontSize);
  ++line;
  const float reciprocalFontSize = 1.0f / fontSize;

  glm::vec2 scale;
  ret = fscanf(fp.get(), " common lineHeight=%*d base=%*d scaleW=%f scaleH=%f pages=%*d packed=%*d", &scale.x, &scale.y);
  if (ret < 2) {
   LOG("ERROR: %s�̓ǂݍ��݂Ɏ��s(line=%d)\n", filename, line);
    return false;
  }
  const glm::vec2 reciprocalScale = glm::vec2(1.0f / scale);
  ++line;

  std::vector<std::string> texNameList;
  for (;;) {
    char tex[128];
    ret = fscanf(fp.get(), " page id=%*d file=%127s", tex);
    if (ret < 1) {
      break;
    }
    std::string texFilename = filename;
    const size_t lastSlashIndex = texFilename.find_last_of('/', std::string::npos);
    if (lastSlashIndex == std::string::npos) {
      texFilename.clear();
    } else {
      texFilename.resize(lastSlashIndex + 1);
    }
    texFilename.append(tex + 1); // �ŏ��́u"�v�𔲂��Ēǉ�.
    texFilename.pop_back(); // �Ō�́u"�v������.
    texNameList.push_back(texFilename);
    ++line;
  }
  if (texNameList.empty()) {
    LOG("ERROR: %s�̓ǂݍ��݂Ɏ��s(line=%d)\n", filename, line);
    return false;
  }

  int charCount;
  ret = fscanf(fp.get(), " chars count=%d", &charCount);
  if (ret < 1) {
    LOG("ERROR: %s�̓ǂݍ��݂Ɏ��s(line=%d)\n", filename, line);
    return false;
  }
  ++line;

  fixedAdvance = 0;
  fontList.resize(65536);
  for (int i = 0; i < charCount; ++i) {
    FontInfo font;
    glm::vec2 uv;
    ret = fscanf(fp.get(), " char id=%d x=%f y=%f width=%f height=%f xoffset=%f yoffset=%f xadvance=%f page=%*d chnl=%*d",
      &font.id, &uv.x, &uv.y, &font.size.x, &font.size.y, &font.offset.x, &font.offset.y, &font.xadvance);
    if (ret < 8) {
      LOG("ERROR: %s�̓ǂݍ��݂Ɏ��s(line=%d)\n", filename, line);
      return false;
    }
    font.offset.y *= -1;
    uv.y = scale.y - uv.y - font.size.y;
    font.uv[0] = uv * reciprocalScale * 65535.0f;
    font.uv[1] = (uv + font.size) * reciprocalScale * 65535.0f;
    if (font.id < 65536) {
      fontList[font.id] = font;
      if (font.xadvance > fixedAdvance) {
        fixedAdvance = font.xadvance;
      }
    }
    ++line;
  }
  texList.clear();
  texList.reserve(texNameList.size());
  for (const auto& e : texNameList) {
    TexturePtr tex = Texture::LoadFromFile(e.c_str());
    if (!tex) {
      return false;
    }
    texList.push_back(tex);
  }
  return true;
}

/**
* ������̉��̃s�N�Z�����𒲂ׂ�.
*/
glm::vec2 Renderer::CalcStringSize(const wchar_t* str) const
{
  glm::vec2 pos(0);
  for (const wchar_t* itr = str; *itr; ++itr) {
    if (*itr == L'\n') {
      pos.x = 0;
      pos.y += 40.0f * scale.y;
    }
    const FontInfo& font = fontList[*itr];
    pos.x += (propotional ? font.xadvance : fixedAdvance) * scale.x;
  }
  return pos;
}

/**
* �������ǉ�����.
*
* @param position �\���J�n���W.
* @param str      �ǉ����镶����.
*
* @retval true  �ǉ�����.
* @retval false �ǉ����s.
*/
bool Renderer::AddString(const glm::vec2& position, const wchar_t* str)
{
  const glm::u16vec2 thicknessAndOutline = glm::vec2(0.625f - thickness * 0.375f, border) * 65535.0f;

  Vertex* p = pVBO + vboSize;
  glm::vec2 pos = position;
  for (const wchar_t* itr = str; *itr; ++itr) {
    if (vboSize + 4 > vboCapacity) {
      break;
    }
    if (*itr == L'\n') {
      pos.y -= 40.0f * reciprocalScreenSize.y * scale.y;
    }
    const FontInfo& font = fontList[*itr];
    if (font.id >= 0 && font.size.x && font.size.y) {
      const glm::vec2 size = font.size * reciprocalScreenSize * scale;
      glm::vec3 offsetedPos = glm::vec3(pos + (font.offset * reciprocalScreenSize) * scale, font.page);
      if (!propotional) {
        offsetedPos.x = pos.x;
      }
      p[0].position = offsetedPos + glm::vec3(0, -size.y, 0);
      p[0].uv = font.uv[0];
      p[0].color = color;
      p[0].subColor = subColor;
      p[0].thicknessAndOutline = thicknessAndOutline;

      p[1].position = offsetedPos + glm::vec3(size.x, -size.y, 0);
      p[1].uv = glm::u16vec2(font.uv[1].x, font.uv[0].y);
      p[1].color = color;
      p[1].subColor = subColor;
      p[1].thicknessAndOutline = thicknessAndOutline;

      p[2].position = offsetedPos + glm::vec3(size.x, 0, 0);
      p[2].uv = font.uv[1];
      p[2].color = color;
      p[2].subColor = subColor;
      p[2].thicknessAndOutline = thicknessAndOutline;

      p[3].position = offsetedPos;
      p[3].uv = glm::u16vec2(font.uv[0].x, font.uv[1].y);
      p[3].color = color;
      p[3].subColor = subColor;
      p[3].thicknessAndOutline = thicknessAndOutline;

      p += 4;
      vboSize += 4;
    }
    pos.x += (propotional ? font.size.x : fixedAdvance) * reciprocalScreenSize.x * scale.x;
  }
  return true;
}

/**
* �����F��ݒ肷��.
*
* @param c �����F.
*/
void Renderer::Color(const glm::vec4& c)
{
  color = glm::clamp(c, 0.0f, 1.0f) * 255.0f;
}

/**
* �����F�擾����.
*
* @return �����F.
*/
glm::vec4 Renderer::Color() const
{
  return glm::vec4(color) * (1.0f / 255.0f);
}

/**
* �T�u�����F��ݒ肷��.
*
* @param c �����F.
*/
void Renderer::SubColor(const glm::vec4& c)
{
  subColor = glm::clamp(c, 0.0f, 1.0f) * 255.0f;
}

/**
* �T�u�����F�擾����.
*
* @return �����F.
*/
glm::vec4 Renderer::SubColor() const
{
  return glm::vec4(subColor) * (1.0f / 255.0f);
}

/**
* VBO���V�X�e���������Ƀ}�b�s���O����.
*/
void Renderer::MapBuffer()
{
  if (pVBO) {
    return;
  }
  glBindBuffer(GL_ARRAY_BUFFER, vbo.Id());
  pVBO = static_cast<Vertex*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vboCapacity, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
  if (!pVBO) {
    const GLenum err = glGetError();
    LOG("ERROR: MapBuffer���s(0x%X).\n", err);
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  vboSize = 0;
}

/**
* VBO�̃}�b�s���O����������.
*/
void Renderer::UnmapBuffer()
{
  if (!pVBO || vboSize == 0) {
    return;
  }
  glBindBuffer(GL_ARRAY_BUFFER, vbo.Id());
  glUnmapBuffer(GL_ARRAY_BUFFER);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  pVBO = nullptr;
}

/**
* �t�H���g��`�悷��.
*/
void Renderer::Draw() const
{
  if (vboSize > 0) {
    vao.Bind();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    progFont->UseProgram();
    for (size_t i = 0; i < texList.size(); ++i) {
      progFont->BindTexture(GL_TEXTURE0 + i, GL_TEXTURE_2D, texList[i]->Id());
    }
    glDrawElements(GL_TRIANGLES, (vboSize / 4) * 6, GL_UNSIGNED_SHORT, 0);
    vao.Unbind();
  }
}

} // namespace Font