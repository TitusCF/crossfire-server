/*****************************************************************************/
/* Animation plugin for Crossfire.                                           */
/*****************************************************************************/
/* Contact: tchize@mailandnews.com                                           */
/*****************************************************************************/
/* This code is placed under the GNU General Public Licence (GPL)            */
/* (C)2001 by tchize (Delbecq David)                                         */
/*****************************************************************************/

#ifndef ANIMATOR_BOX_H
#define ANIMATOR_BOX_H
CFmovement* parse_animation_block (char* buffer, size_t buffer_size,FILE* fichier, CFanimation* parent);
#endif
