#include "global.h"

Spell merge(Spell lhs, Spell rhs) {
  return (Spell){
      .class = lhs.class | rhs.class,
      .castTime = 1.2 * lhs.castTime + rhs.castTime,
      .damage = lhs.damage + rhs.damage,
      .type = lhs.type + rhs.type,
  };
}
